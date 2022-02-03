using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace Trivia_client
{
    public partial class Game_Results : Window
    {
        private NetworkStream clientStream;
        private string username;
        private string server_answer;
        public Game_Results(string username, NetworkStream clientStream, string server_answer)
        {
            InitializeComponent();

            this.clientStream = clientStream;
            this.username = username;
            this.server_answer = server_answer;

            fill_results();
        }

        private void fill_results()
        {
            int users_number;
            string username;
            int username_size;
            string score;

            users_number = int.Parse(server_answer.Substring(0, 1));
            server_answer = server_answer.Substring(1);

            for (int i = 0; i < users_number; i++)
            {
                username_size = int.Parse(server_answer.Substring(0, 2));
                server_answer = server_answer.Substring(2);

                username = server_answer.Substring(0, username_size);
                server_answer = server_answer.Substring(username_size);

                score = server_answer.Substring(0, 2);
                server_answer = server_answer.Substring(2);

                Button newButton = new Button();
                newButton.Content = username + " - " + score;
                newButton.Name = "btn_" + username;

                results_list.Items.Add(newButton);
            }
        }

        private void Continue(object sender, RoutedEventArgs e)
        {
            Menu menu = new Menu(username, clientStream);
            this.Close();
            menu.Show();
        }
    }
}
