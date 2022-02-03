using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
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
    public partial class Menu : Window
    {
        private NetworkStream clientStream;
        private string username;
        public Menu(string username, NetworkStream clientStream)
        {
            InitializeComponent();
            this.clientStream = clientStream;
            this.username = username;

            username_label.Content = "Hello " + username;

            fill_room_list();
            fill_statics();
        }

        private void fill_room_list()
        {
            rooms_list.Items.Clear();

            int count_rooms;
            string room_id;
            int room_name_size;
            string room_name;

            byte[] buffer = new ASCIIEncoding().GetBytes("205");
            clientStream.Write(buffer, 0, buffer.Length);
            clientStream.Flush();

            buffer = new byte[4096];
            int bytesRead = clientStream.Read(buffer, 0, 4096);

            string server_answer = Encoding.ASCII.GetString(buffer.ToArray(), 0, (int)buffer.Length);
            server_answer = server_answer.Substring(3);

            count_rooms = int.Parse(server_answer.Substring(0, 4));
            server_answer = server_answer.Substring(4);

            for (int i = 0; i < count_rooms; i++)
            {
                room_id = server_answer.Substring(0, 4);
                server_answer = server_answer.Substring(4);

                room_name_size = int.Parse(server_answer.Substring(0, 2));
                server_answer = server_answer.Substring(2);

                room_name = server_answer.Substring(0, room_name_size);
                server_answer = server_answer.Substring(room_name_size);

                Button newButton = new Button();
                newButton.Content = room_name + " - " + room_id;

                newButton.Click += join_room;

                newButton.Name = "btn" + room_id;

                rooms_list.Items.Add(newButton);
            }
        }

        private void fill_statics()
        {
            int num_games;
            int time_ans;
            int right_ans;
            int wrong_ans;

            byte[] buffer = new ASCIIEncoding().GetBytes("225");
            clientStream.Write(buffer, 0, buffer.Length);
            clientStream.Flush();

            buffer = new byte[4096];
            int bytesRead = clientStream.Read(buffer, 0, 4096);

            string server_answer = Encoding.ASCII.GetString(buffer.ToArray(), 0, (int)buffer.Length);
            server_answer = server_answer.Substring(3);

            num_games = int.Parse(server_answer.Substring(0, 4));
            server_answer = server_answer.Substring(4);

            right_ans = int.Parse(server_answer.Substring(0, 6));
            server_answer = server_answer.Substring(6);

            wrong_ans = int.Parse(server_answer.Substring(0, 6));
            server_answer = server_answer.Substring(6);

            time_ans = int.Parse(server_answer.Substring(0, 4));
            server_answer = server_answer.Substring(4);

            num_games_label.Content = "Number Games: " + num_games;
            right_ans_label.Content = "Number Right Answers: " + right_ans;
            wrong_ans_label.Content = "Number Wrong Answers: " + wrong_ans;
            time_ans_label.Content = "Average Time Answer: " + time_ans;
        }

        private void join_room(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;

            int room_id = int.Parse(button.Name.Substring(3));

            byte[] buffer = new ASCIIEncoding().GetBytes("209" + room_id.ToString().PadLeft(4, '0'));
            clientStream.Write(buffer, 0, buffer.Length);
            clientStream.Flush();

            buffer = new byte[4096];
            int bytesRead = clientStream.Read(buffer, 0, 4096);

            string server_answer = Encoding.ASCII.GetString(buffer.ToArray(), 0, (int)buffer.Length);
            server_answer = server_answer.Substring(3);

            if (server_answer[0] == '0')
            {
                WaitingRoom waitingRoom = new WaitingRoom(room_id, username, false, clientStream, int.Parse(server_answer.Substring(1, 3)), int.Parse(server_answer.Substring(3, 5)));
                this.Close();
                waitingRoom.Show();
            }
            else if(server_answer[0] == '1')
            {
                status_textbox.Content = "Status: room is full";
            }
            else if (server_answer[0] == '2')
            {
                status_textbox.Content = "Status: room not exist or other reason";
            }
        }

        private void update_list(object sender, RoutedEventArgs e)
        {
            fill_room_list();
        }
        private void update_statistics(object sender, RoutedEventArgs e)
        {
            fill_statics();
        }

        private void create_room(object sender, RoutedEventArgs e)
        {
            string room_name = "";
            if (room_name_textbox.Text == "")
            {
                status_textbox.Content = "Status: wrong room name";
                return;
            }
            else
            {
                room_name = room_name_textbox.Text;
            }

            int player_number = 0;
            if (player_number_textbox.Text != "" && int.TryParse(player_number_textbox.Text, out int n1) && int.Parse(player_number_textbox.Text) > 0 && int.Parse(player_number_textbox.Text) < 10)
            {
                player_number = int.Parse(player_number_textbox.Text);
            }
            else {
                status_textbox.Content = "Status: wrong player number";
                return;
            }

            string question_number = "";
            if (question_number_textbox.Text != "" && int.TryParse(question_number_textbox.Text, out int n2) && int.Parse(question_number_textbox.Text) > 0 && int.Parse(question_number_textbox.Text) < 6)
            {
                question_number = question_number_textbox.Text.ToString().PadLeft(2, '0');
            }
            else
            {
                status_textbox.Content = "Status: wrong question number";
                return;
            }

            string question_time = "";
            if (question_time_textbox.Text != "" && int.TryParse(question_time_textbox.Text, out int n3) && int.Parse(question_time_textbox.Text) > 4 && int.Parse(question_time_textbox.Text) < 61)
            {
                question_time = question_time_textbox.Text.ToString().PadLeft(2, '0');
            }
            else
            {
                status_textbox.Content = "Status: wrong question time";
                return;
            }

            string room_name_length = room_name.Length.ToString().PadLeft(2, '0');

            byte[] buffer = new ASCIIEncoding().GetBytes("213" + room_name_length + room_name + player_number + question_number + question_time);
            clientStream.Write(buffer, 0, buffer.Length);
            clientStream.Flush();

            buffer = new byte[4096];
            int bytesRead = clientStream.Read(buffer, 0, 4096);

            String server_answer = Encoding.ASCII.GetString(buffer.ToArray(), 0, (int)buffer.Length);

            if (server_answer[3] == '0')
            {
                status_textbox.Content = "Status: Succsses to create room";
                WaitingRoom waitingRoom = new WaitingRoom(int.Parse(server_answer.Substring(3, 5)), username, true, clientStream, int.Parse(question_number), int.Parse(question_time));
                this.Close();
                waitingRoom.Show();
            }
            else if (server_answer[3] == '1')
            {
                status_textbox.Content = "Status: Fail to create room";
            }
        }

        private void best_results(object sender, RoutedEventArgs e)
        {
            Best_Results best_results = new Best_Results(username, clientStream);
            this.Close();
            best_results.Show();
        }

        private void sign_out(object sender, RoutedEventArgs e)
        {
            byte[] buffer = new ASCIIEncoding().GetBytes("201");
            clientStream.Write(buffer, 0, buffer.Length);
            clientStream.Flush();

            MainWindow main_window = new MainWindow();
            this.Close();
            main_window.Show();
        }

        private void print_server(object sender, RoutedEventArgs e)
        {
            byte[] buffer = new ASCIIEncoding().GetBytes("300");
            clientStream.Write(buffer, 0, buffer.Length);
            clientStream.Flush();
        }
    }
}
