using NetworkTables;
using NetworkTables.Tables;
using AForge.Video;
using System.Drawing;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Media.Imaging;
using System.IO;
using System.Drawing.Imaging;
using System;

namespace FRCDashboard2019
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, IRemoteConnectionListener, ITableListener
    {
        private List<string> autonOptions = new List<string>();
        private NetworkTable table = NetworkTable.GetTable(@"DASHBOARD_TABLE");
        private List<string> positionOptions = new List<string>();

        //static string addr = "http://localhost:8000/camera/mjpeg";  //streaming my webcam right using cam2web
        static string addr = "http://10.34.81.11:8000/camera/mjpeg"; //panda address for using cam2web.
        //static string addr = "http://10.34.81.2:1181/?action=stream"; //robiorio usb camera address
        private MJPEGStream stream = new MJPEGStream(addr);

        public MainWindow()
        {
            InitializeComponent();
            table.AddTableListener(this, true);
            table.AddConnectionListener(this, true);
            stream.NewFrame += new NewFrameEventHandler(Video_NewFrame);
            stream.VideoSourceError += new VideoSourceErrorEventHandler(Video_VideoSourceError);
            this.Closed += MainWindow_Closed;
            stream.Start();
#if DEBUG
            VersionLabel.Content = "DEBUG";
#else
            VersionLabel.Content = "RELEASE";
#endif
        }

        private void MainWindow_Closed(object sender, System.EventArgs e)
        {
            Environment.Exit(0);
        }

        private void Video_VideoSourceError(object sender, VideoSourceErrorEventArgs eventArgs)
        {
            if(eventArgs.Description.Equals("Unable to connect to the remote server"))
            {
                videoStatusLabel.Dispatcher.Invoke(() =>
                {
                    videoStatusLabel.Content = "DISCONNECTED";
                    videoStatusLabel.Foreground = System.Windows.Media.Brushes.Red;
                });
            }
            else if(eventArgs.Description.Equals("The operation has timed out"))
            {
                videoStatusLabel.Dispatcher.Invoke(() =>
                {
                    videoStatusLabel.Content = "TIMED OUT";
                    videoStatusLabel.Foreground = System.Windows.Media.Brushes.Orange;
                });
            }
            else
                MessageBox.Show(eventArgs.Description);
        }

        private void Video_NewFrame(object sender, NewFrameEventArgs eventArgs)
        {
            CameraFeedImage.Dispatcher.Invoke(() =>
            {
                Bitmap bitmap = eventArgs.Frame;
                BitmapImage bitmapImage = BitmapToBitmapImage(bitmap);
                CameraFeedImage.Source = bitmapImage;
            });
            videoStatusLabel.Dispatcher.Invoke(() =>
            {
                videoStatusLabel.Content = "CONNECTED";
                videoStatusLabel.Foreground = System.Windows.Media.Brushes.Green;
            });
        }

        //code found from deep internet digging
        public static BitmapImage BitmapToBitmapImage(Bitmap img) /* Convert from System.Drawing.Bitmap to Windows.Media.Image */
        {
            BitmapImage bi = new BitmapImage();
            bi.BeginInit();

            MemoryStream ms = new MemoryStream();
            img.Save(ms, ImageFormat.Bmp);
            ms.Seek(0, SeekOrigin.Begin);

            bi.StreamSource = ms;
            bi.EndInit();
            return bi;
        }

        public void Connected(IRemote remote, ConnectionInfo info)
        {
            //thread save call to the UI
            connectionStatusLabel.Dispatcher.Invoke(() =>
            {
                connectionStatusLabel.Foreground = System.Windows.Media.Brushes.Green;
                connectionStatusLabel.Content = $@"CONNECTED @ {info.RemoteIp}";
            });
        }

        public void Disconnected(IRemote remote, ConnectionInfo info)
        {
            //thread save call to the UI
            connectionStatusLabel.Dispatcher.Invoke(() =>
            {
                connectionStatusLabel.Foreground = System.Windows.Media.Brushes.Red;
                connectionStatusLabel.Content = @"DISCONNECTED";
            });
        }

        private void updateAutonOptions(string[] options)
        {
            //thread save call to the UI
            autonSelectionCombo.Dispatcher.Invoke(() =>
            {
                autonOptions.Clear();
                autonOptions.AddRange(options);
                autonSelectionCombo.ItemsSource = autonOptions;
                
            });
        }

        private void updatePositionOptions(string[] options)
        {
            positionSelectionCombo.Dispatcher.Invoke(() =>
            {
                positionOptions.Clear();
                positionOptions.AddRange(options);
                positionSelectionCombo.ItemsSource = positionOptions;
            });
        }

        private void updateAutonFound(string s)
        {
            CorrectAuton.Dispatcher.Invoke(() =>
            {
                CorrectAuton.Content = s;
            });
        }

        public void ValueChanged(ITable source, string key, Value value, NotifyFlags flags)
        {
            switch (key)
            {
                case @"AUTON_OPTIONS":
                    updateAutonOptions(value.GetStringArray());
                    break;
                case @"POSITION_OPTIONS":
                    updatePositionOptions(value.GetStringArray());
                    break;
                case @"AUTON_FOUND":
                    updateAutonFound(value.GetString());
                    break;
            }
        }

        private void AutonSelectionCombo_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            table.PutString(@"AUTON_SELECTION", (string)autonSelectionCombo.SelectedItem);
        }
        private void PositionSelectionCombo_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            table.PutString(@"POSITION_SELECTION", (string)positionSelectionCombo.SelectedItem);
        }

    }
}
