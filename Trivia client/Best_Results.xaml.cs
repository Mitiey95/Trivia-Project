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
    public partial class Best_Results : Window
    {
        private NetworkStream clientStream;
        private string username;
        public Best_Results(string username, NetworkStream clientStream)
        {
            InitializeComponent();

            this.clientStream = clientStream;
            this.username = username;

            fill_results();
        }

        private void fill_results()
        {
            byte[] buffer = new ASCIIEncoding().GetBytes("223");
            clientStream.Write(buffer, 0, buffer.Length);
            clientStream.Flush();

            buffer = new byte[4096];
            int bytesRead = clientStream.Read(buffer, 0, 4096);

            string server_answer = Encoding.ASCII.GetString(buffer.ToArray(), 0, (int)buffer.Length);
            server_answer = server_answer.Substring(3);

            string user1 = "";
            int user1_size;
            string user1_score = "";

            string user2 = "";
            int user2_size;
            string user2_score = "";

            string user3 = "";
            int user3_size;
            string user3_score = "";

            user1_size = int.Parse(server_answer.Substring(0, 2));
            server_answer = server_answer.Substring(2);
            user1 = server_answer.Substring(0, user1_size);
            server_answer = server_answer.Substring(user1_size);
            user1_score = server_answer.Substring(0, 6);
            server_answer = server_answer.Substring(6);

            user2_size = int.Parse(server_answer.Substring(0, 2));
            server_answer = server_answer.Substring(2);
            user2 = server_answer.Substring(0, user2_size);
            server_answer = server_answer.Substring(user2_size);
            user2_score = server_answer.Substring(0, 6);
            server_answer = server_answer.Substring(6);

            user3_size = int.Parse(server_answer.Substring(0, 2));
            server_answer = server_answer.Substring(2);
            user3 = server_answer.Substring(0, user3_size);
            server_answer = server_answer.Substring(user3_size);
            user3_score = server_answer.Substring(0, 6);
            server_answer = server_answer.Substring(6);

            label1.Content += "\t" + user1 + "  " + user1_score;
            label2.Content += "\t" + user2 + "  " + user2_score;
            label3.Content += "\t" + user3 + "  " + user3_score;

        }

        private void Continue(object sender, RoutedEventArgs e)
        {
            Menu menu = new Menu(username, clientStream);
            this.Close();
            menu.Show();
        }
    }
}
