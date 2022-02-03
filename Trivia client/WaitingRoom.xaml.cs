using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Windows.Threading;

namespace Trivia_client
{
    public partial class WaitingRoom : Window
    {
        private int room_id;
        private string username;
        private bool isAdmin;
        private NetworkStream clientStream;
        private int question_number;
        private int question_time;

        private Thread thread;

        public WaitingRoom(int room_id, string username, bool isAdmin, NetworkStream clientStream, int question_number, int question_time)
        {
            InitializeComponent();

            this.room_id = room_id;
            this.username = username;
            this.isAdmin = isAdmin;
            this.clientStream = clientStream;
            this.question_number = question_number;
            this.question_time = question_time;

            fill_users_list();

            thread = new Thread(new ThreadStart(listen_server));
            thread.SetApartmentState(ApartmentState.STA);
            thread.Start();

            if (!isAdmin)
            {
                start_game_button.Visibility = Visibility.Hidden;
            }
        }

        private void fill_users_list()
        {
            byte[] buffer = new ASCIIEncoding().GetBytes("207" + room_id.ToString().PadLeft(4, '0'));
            clientStream.Write(buffer, 0, buffer.Length);
            clientStream.Flush();
        }

        private void update_users_list(object sender, RoutedEventArgs e)
        {
            fill_users_list();
        }
        private void leave_room(object sender, RoutedEventArgs e)
        {
            if (isAdmin)
            {
                byte[] buffer = new ASCIIEncoding().GetBytes("215");
                clientStream.Write(buffer, 0, buffer.Length);
                clientStream.Flush();
            }
            else
            {
                byte[] buffer = new ASCIIEncoding().GetBytes("211");
                clientStream.Write(buffer, 0, buffer.Length);
                clientStream.Flush();
            }
        }

        private void listen_server()
        {
            while (true)
            {
                byte[] buffer = new byte[4096];
                int bytesRead = clientStream.Read(buffer, 0, 4096);

                string server_answer = Encoding.ASCII.GetString(buffer.ToArray(), 0, (int)buffer.Length);

                this.Dispatcher.Invoke(() => {

                    if (server_answer.Substring(0, 3) == "112" || server_answer.Substring(0, 3) == "116")
                    {
                        Menu menu = new Menu(username, clientStream);

                        this.Close();
                        menu.Show();

                        thread.Abort();
                    }
                    else if (server_answer.Substring(0, 3) == "108")
                    {
                        users_list.Items.Clear();

                        int count_users;
                        string username;
                        int username_size;

                        server_answer = server_answer.Substring(3);

                        count_users = int.Parse(server_answer.Substring(0, 1));
                        server_answer = server_answer.Substring(1);

                        for (int i = 0; i < count_users; i++)
                        {
                            username_size = int.Parse(server_answer.Substring(0, 2));
                            server_answer = server_answer.Substring(2);

                            username = server_answer.Substring(0, username_size);
                            server_answer = server_answer.Substring(username_size);

                            Button newButton = new Button();
                            newButton.Content = username;
                            newButton.Name = "btn" + i;

                            users_list.Items.Add(newButton);
                        }
                    }
                    else if (server_answer.Substring(0, 3) == "118")
                    {
                        server_answer = server_answer.Substring(3);

                        int question_size = int.Parse(server_answer.Substring(0, 3));
                        server_answer = server_answer.Substring(3);
                        string question = server_answer.Substring(0, question_size);
                        server_answer = server_answer.Substring(question_size);

                        int answer1_size = int.Parse(server_answer.Substring(0, 3));
                        server_answer = server_answer.Substring(3);
                        string answer1 = server_answer.Substring(0, answer1_size);
                        server_answer = server_answer.Substring(answer1_size);

                        int answer2_size = int.Parse(server_answer.Substring(0, 3));
                        server_answer = server_answer.Substring(3);
                        string answer2 = server_answer.Substring(0, answer2_size);
                        server_answer = server_answer.Substring(answer2_size);

                        int answer3_size = int.Parse(server_answer.Substring(0, 3));
                        server_answer = server_answer.Substring(3);
                        string answer3 = server_answer.Substring(0, answer3_size);
                        server_answer = server_answer.Substring(answer3_size);

                        int answer4_size = int.Parse(server_answer.Substring(0, 3));
                        server_answer = server_answer.Substring(3);
                        string answer4 = server_answer.Substring(0, answer4_size);
                        server_answer = server_answer.Substring(answer4_size);

                        Game_Window game_window = new Game_Window(username, clientStream, isAdmin, question, answer1, answer2, answer3, answer4, question_time);

                        this.Close();
                        game_window.Show();

                        thread.Abort();
                    }
                });
            }
        }

        private void Start_Game(object sender, RoutedEventArgs e)
        {
            byte[] buffer = new ASCIIEncoding().GetBytes("217");
            clientStream.Write(buffer, 0, buffer.Length);
            clientStream.Flush();
        }
    }
}
