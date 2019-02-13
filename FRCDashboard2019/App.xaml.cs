using NetworkTables;
using System.Windows;

namespace FRCDashboard2019
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        public App()
        {
            NetworkTable.SetClientMode();
            NetworkTable.SetIPAddress(@"127.0.0.1");
            NetworkTable.SetNetworkIdentity(@"BRONCBOTZ_DASHBOARD2");
            NetworkTable.Initialize();
        }
    }
}
