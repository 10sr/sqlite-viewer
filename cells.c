/* example-start helloworld helloworld.c */

#include <gtk/gtk.h>

/* これはコールバック関数。引数 data はこの例では使用していない。
   以下、コールバック関数の定義が続く。 */

void hello( GtkWidget *widget,
            gpointer   data )
{
  g_print ("Hello World\n");
}

gint delete_event( GtkWidget *widget,
                   GdkEvent  *event,
                   gpointer   data )
{
  /* "delete_event" のシグナルハンドラから FALSE を返すと、
     GTK は "destroy (破壊)" シグナルを発行する。
     TRUE を返すと、このウィンドウを破壊して欲しくないことを
     意味する。これは、「本当に終了していいですか？」と
     ポップアップ表示するような場合に便利である。
  */

  g_print ("delete event occurred\n");

  /* この TRUE を FALSE に変えると、メインウィンドウは
     "delete_event" で終了するようになる。*/

  return(TRUE);
}

/* もうひとつのコールバック */
void destroy( GtkWidget *widget,
              gpointer   data )
{
  gtk_main_quit();
}

int main( int   argc,
          char *argv[] )
{
  /* GtkWidget はウィジェット用のストレージクラスである */
  GtkWidget *window;
  GtkWidget *button;

  /* これは全ての GTK アプリケーションに必要な呼び出しである。
     コマンドラインからの引数をパースした後、引数はアプリケーション
     に返される。*/
  gtk_init(&argc, &argv);

  /* 新しくウィンドウを作る */
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

  /* ウィンドウへ "delete_event" シグナルが送られたら(これは通常、
     ウィンドウネージャから "close" オプションやタイトルバーの操作に
     よって送られる)、上で定義した関数 delete_event() を呼出すように
     設定する。コールバック関数へのデータは NULL で、コールバック
     関数内では使用していない。*/
  gtk_signal_connect (GTK_OBJECT (window), "delete_event",
                      GTK_SIGNAL_FUNC (delete_event), NULL);

  /* ここでは "destroy" イベントをシグナルハンドラに接続する。
     このイベントは、このウィンドウにおいて gtk_widget_destroy() を
     呼出すか、"delete_event" コールバックが FALSE を返すと発生する。 */
  gtk_signal_connect (GTK_OBJECT (window), "destroy",
                      GTK_SIGNAL_FUNC (destroy), NULL);

  /* ウィンドウのボーダー幅を設定する。*/
  gtk_container_set_border_width (GTK_CONTAINER (window), 10);

  /* 新しく "Hello World" というラベルのついたボタンを作成する */
  button = gtk_button_new_with_label ("Hello World");

  /* ボタンが "clicked" シグナルを受け取ると、hello() の引数に NULL
     を渡して呼出すように設定する。関数 hello() は上で定義している。*/
  gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      GTK_SIGNAL_FUNC (hello), NULL);

  /* "clicked" シグナルが発行されたら、gtk_widget_destory(window) を
     呼出してウィンドウを破壊するよう設定している。繰り返しになるが、
     "destroy" シグナルは、ここか、ウィンドウマネージャから発生する。*/
  gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
                             GTK_SIGNAL_FUNC (gtk_widget_destroy),
                             GTK_OBJECT (window));

  /* ボタンをウィンドウ(gtk コンテナ)にパックする */
  gtk_container_add (GTK_CONTAINER (window), button);

  /* 最後に新しく作成したウィジェットを表示する */
  gtk_widget_show (button);

  /* window も表示する */
  gtk_widget_show (window);

  /* 全ての GTK アプリケーションには必ず gtk_main() がある。
     処理はここで、イベント(キープレスやマウスイベントなど)が
     発生するのを待って中断する。*/
  gtk_main ();

  return(0);
}
/* example-end */
