/* example-start scrolledwin scrolledwin.c */

#include <gtk/gtk.h>

void destroy(GtkWidget *widget, gpointer data)
{
    gtk_main_quit();
}

int main (int argc, char *argv[])
{
    static GtkWidget *window;
    GtkWidget *scrolled_window;
    GtkWidget *table;
    GtkWidget *button;
    char buffer[32];
    int i, j;

    gtk_init (&argc, &argv);

    /* スクロールウィンドウをパックするためのダイアログを
     * 新規に作成する。*/
    window = gtk_dialog_new ();
    gtk_signal_connect (GTK_OBJECT (window), "destroy",
                        (GtkSignalFunc) destroy, NULL);
    gtk_window_set_title (GTK_WINDOW (window), "GtkScrolledWindow example");
    gtk_container_set_border_width (GTK_CONTAINER (window), 0);
    gtk_widget_set_usize(window, 300, 300);

    /* 新しくスクロールドウィンドウを作成 */
    scrolled_window = gtk_scrolled_window_new (NULL, NULL);

    gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 10);

    /* ポリシーは GTK_POLICY AUTOMATIC か GTK_POLICY_ALWAYS のどちらかである。
     * GTK_POLICY_AUTOMATIC は自動的にスクロールバーが必要か判断するのに対し、
     * GTK_POLICY_ALWAYS は常にスクロールバーを表示する。
     * 最初に設定しているのが水平方向で、次が垂直方向である。*/

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

    /* ダイアログウィンドウは内部に vbox を持っている */
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG(window)->vbox), scrolled_window,
                        TRUE, TRUE, 0);
    gtk_widget_show (scrolled_window);

    /* 10 x 10 の 正方形の表を作成 */
    table = gtk_table_new (10, 10, FALSE);

    /* x 方向と y 方向のスペーシングを 10 に設定 */
    gtk_table_set_row_spacings (GTK_TABLE (table), 10);
    gtk_table_set_col_spacings (GTK_TABLE (table), 10);

    /* テーブルをスクロールドウィンドウにパック */
    gtk_scrolled_window_add_with_viewport (
                   GTK_SCROLLED_WINDOW (scrolled_window), table);
    gtk_widget_show (table);

    /* これはスクロールドウィンドウのデモ用に、トグルボタンのテーブル
     * を作成しているだけ。*/
    for (i = 0; i < 10; i++)
       for (j = 0; j < 10; j++) {
          sprintf (buffer, "button (%d,%d)\n", i, j);
          button = gtk_toggle_button_new_with_label (buffer);
          gtk_table_attach_defaults (GTK_TABLE (table), button,
                                     i, i+1, j, j+1);
          gtk_widget_show (button);
       }

    /* "close" ボタンをダイアログの下に追加 */
    button = gtk_button_new_with_label ("close");
    gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
                               (GtkSignalFunc) gtk_widget_destroy,
                               GTK_OBJECT (window));

    /* このボタンをデフォルトに指定 */

    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_box_pack_start (GTK_BOX (GTK_DIALOG (window)->action_area), button, TRUE, TRUE, 0);

    /* この操作でこのボタンをデフォルトのボタンにする。"Enter" キーを
       押すだけでボタンが押される。 */
    gtk_widget_grab_default (button);
    gtk_widget_show (button);

    gtk_widget_show (window);

    gtk_main();

    return(0);
}
/* example-end */
