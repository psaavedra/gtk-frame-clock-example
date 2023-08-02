#include <gtk/gtk.h>


#include <gdk/gdkwayland.h>
#include <wayland-client.h>

#include <time.h>

#define LOG(...) fprintf(stderr, __VA_ARGS__);\
                         fprintf(stderr, "\n")

long long previousDrawTime, currentDrawTime;
long long previousTickTime, currentTickTime;

GdkFrameClock *frame_clock = NULL;

struct wl_surface *surface = NULL;

gint64 frame_counter = 0;

gint64 update_frame_counter() {
    if (frame_clock) {
        frame_counter = gdk_frame_clock_get_frame_counter(frame_clock);
        return frame_counter;
    }
    return 0;
}

gint64 get_time_now() {
    LOG("               get timings:");
    struct timespec currentTime;

    // Get the current time from the monotonic clock
    if (clock_gettime(CLOCK_MONOTONIC, &currentTime) == -1) {
        perror("clock_gettime");
        return 1;
    }

    // Calculate the time in microseconds
    gint64 microseconds = currentTime.tv_sec * 1000000LL + currentTime.tv_nsec / 1000LL;
    LOG("               |  - now: %ld", microseconds);

    if (frame_clock) {
        // LOG("                      frame time: %ld", gdk_frame_clock_get_frame_time(frame_clock));
        // return gdk_frame_clock_get_frame_time(frame_clock);

        update_frame_counter();
        gint64 frame_time = gdk_frame_clock_get_frame_time(frame_clock);
        LOG("               |  - frame time: %ld (counter: %ld) (frame time - now: %ld)", frame_time, frame_counter, frame_time - microseconds);

        GdkFrameTimings* timings = gdk_frame_clock_get_timings(frame_clock, frame_counter);

        // gboolean timings_complete = gdk_frame_timings_get_complete(timings);
        // LOG("                 - timings_complete: %d", timings_complete);
        // gint64 presentation_time = gdk_frame_timings_get_presentation_time(timings);
        // LOG("                 - presentation_time: %ld", presentation_time);
        gint64 predicted_presentation_time = gdk_frame_timings_get_predicted_presentation_time(timings);
        LOG("               |  - predicted presentation time: %ld (predicted - now: %ld)", predicted_presentation_time, predicted_presentation_time - microseconds);
        LOG("               \\");
    }

    return microseconds;
}

static void on_commit(void *data, struct wl_callback *callback, uint32_t time) {
    LOG("%ld:  wl_surface:on_commit", get_time_now());
    // wl_callback_destroy(callback);
        GdkFrameTimings* timings = gdk_frame_clock_get_timings(frame_clock, frame_counter);

    // if (frame_clock) {
    //     gboolean timings_complete = gdk_frame_timings_get_complete(timings);
    //     LOG("                 - timings_complete: %d", timings_complete);
    //     gint64 presentation_time = gdk_frame_timings_get_presentation_time(timings);
    //     LOG("                 - presentation_time: %ld", presentation_time);
    //     gint64 predicted_presentation_time = gdk_frame_timings_get_predicted_presentation_time(timings);
    // }
    LOG("(e): End of cycle");
}

static const struct wl_callback_listener commit_listener = {
    on_commit,
};

/*
 * This signal is emitted when a widget is supposed to render itself.
 */
static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data) {
    // Your drawing operations here
    cairo_set_source_rgb(cr, (double)rand()/RAND_MAX, (double)rand()/RAND_MAX, (double)rand()/RAND_MAX);
    cairo_paint(cr);

    previousDrawTime = currentDrawTime;
    currentDrawTime = get_time_now();
    LOG("%lld:  widget:on_draw (tick-draw latency: %lld)", currentDrawTime, currentDrawTime - currentTickTime);

    struct wl_surface *surface = gdk_wayland_window_get_wl_surface(gtk_widget_get_window(widget));
    // LOG("                 - surface: %p", surface);
    struct wl_callback *callback = wl_surface_frame(surface);
    wl_callback_add_listener(callback, &commit_listener, NULL);

    return FALSE;
}

static int on_tick_callback(GtkWidget *widget, GdkFrameClock *frame_clock, gpointer user_data) {
    gtk_widget_queue_draw(GTK_WIDGET(user_data));
    previousTickTime = currentTickTime;
    currentTickTime = get_time_now();
    LOG("%lld:  widget:on_tick_callback (rate: %lld)", currentTickTime, currentTickTime - previousTickTime);
    return G_SOURCE_CONTINUE;
}

static void on_after_paint(GdkFrameClock* clock, gpointer user_data) {
    LOG("               clock:on_after_paint");
}

static void on_before_paint(GdkFrameClock* clock, gpointer user_data) {
    LOG("(s): Cycle start");
    LOG("               clock:on_before_paint");
}

static void on_flush_events(GdkFrameClock* clock, gpointer user_data) {
    LOG("               clock:on_flush_events");
}

static void on_layout(GdkFrameClock* clock, gpointer user_data) {
    LOG("               clock:on_layout");
}

static void on_paint(GdkFrameClock* clock, gpointer user_data) {
    LOG("               clock:on_paint");
}

static void on_update(GdkFrameClock* clock, gpointer user_data) {
    LOG("               clock:on_update");
}

static void on_realize(GtkWidget* widget, gpointer user_data) {
    LOG(" widget:on_realize");

    frame_clock = gtk_widget_get_frame_clock(widget);
    LOG("- frame_clock : %p", frame_clock);
    g_signal_connect(frame_clock, "after-paint", G_CALLBACK(on_after_paint), NULL);
    g_signal_connect(frame_clock, "before-paint", G_CALLBACK(on_before_paint), NULL);
    g_signal_connect(frame_clock, "flush-events", G_CALLBACK(on_flush_events), NULL);
    g_signal_connect(frame_clock, "layout", G_CALLBACK(on_layout), NULL);
    g_signal_connect(frame_clock, "paint", G_CALLBACK(on_paint), NULL);
    g_signal_connect(frame_clock, "update", G_CALLBACK(on_update), NULL);

    // LOG("window: %p", window);
    LOG("- window: %p", gtk_widget_get_window(widget));
    struct wl_surface *surface = gdk_wayland_window_get_wl_surface(gtk_widget_get_window(widget));
    LOG("- surface: %p", surface);
    // struct wl_callback *callback = wl_surface_frame(surface);
    // wl_callback_add_listener(callback, &commit_listener, NULL);
}

gboolean on_frame_clock(GtkWidget *widget, GdkFrameClock *frame_clock, gpointer data) {
    // This function will be called for each frame (repaint) of the widget

    // Do your painting or processing here
    // For simplicity, we will just invalidate the widget (request redraw)
    // gtk_widget_queue_draw(widget);
    LOG("               widget:on_frame_clock");

    return G_SOURCE_CONTINUE; // Keep receiving frame callbacks
}

int main(int argc, char *argv[]) {
    // Seed the random number generator with the current time
    srand(time(NULL));

    gtk_init(&argc, &argv);
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    // g_signal_connect(window, "realize", G_CALLBACK(on_realize), NULL);
    // g_signal_connect(window, "frame-clock", G_CALLBACK(on_frame_clock), NULL);
    // gtk_widget_add_tick_callback(window, on_tick_callback, window, NULL);

    GtkWidget *drawing_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(window), drawing_area);

    g_signal_connect(drawing_area, "realize", G_CALLBACK(on_realize), NULL);
    gtk_widget_add_tick_callback(GTK_WIDGET(drawing_area), on_tick_callback, drawing_area, NULL);

    g_signal_connect(drawing_area, "draw", G_CALLBACK(on_draw), NULL);

    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
