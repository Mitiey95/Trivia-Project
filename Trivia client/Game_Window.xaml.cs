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
    /// <summary>
    /// Interaction logic for Game_Window.xaml
    /// </summary>
    public partial class Game_Window : Window
    {
        private string username;
        private NetworkStream clientStream;
        private bool isAdmin;

        private bool isUserAnswered = false;
        private string answer_number = "5";
        private int isCorrect = -1;     // -1 - NO ANSWER | 0 - INCORRECT | 1 - CORRECT

        private int num_correct_answers = 0;
        private int time_results = 2;

        DispatcherTimer _timer = new System.Windows.Threading.DispatcherTimer();

        private int question_time_const;
        private int question_time;
        int seconds_pass = 0;

        private string question;
        private string answer1;
        private string answer2;
        private string answer3;
        private string answer4;

        Thread thread;
        public Game_Window(string username, NetworkStream clientStream, bool isAdmin, string question, string answer1, string answer2, string answer3, string answer4, int question_time)
        {
            InitializeComponent();

            this.username = username;
            this.clientStream = clientStream;
            this.isAdmin = isAdmin;

            question_label.Content = question;
            ans1_button.Content = answer1;
            ans2_button.Content = answer2;
            ans3_button.Content = answer3;
            ans4_button.Content = answer4;

            this.question_time = question_time;
            this.question_time_const = question_time;
            timer_label.Content = question_time;

            ans1_button.Background = Brushes.White;
            ans2_button.Background = Brushes.White;
            ans3_button.Background = Brushes.White;
            ans4_button.Background = Brushes.White;

            _timer = new DispatcherTimer
            {
                Interval = new TimeSpan(0, 0, 1)
            };
            _timer.Tick += timer_tick;
            _timer.Start();

            thread = new Thread(new ThreadStart(next_question));
            thread.SetApartmentState(ApartmentState.STA);
            thread.Start();
        }

        private void timer_tick(object sender, EventArgs e)
        {
            if (question_time > 0)
            {
                question_time--;
                timer_label.Content = question_time;

                if (!isUserAnswered)
                {
                    seconds_pass++;
                }
            }
            else
            {
                if (question_time == 0)
                {
                    byte[] buffer = new ASCIIEncoding().GetBytes("219" + answer_number + seconds_pass.ToString().PadLeft(2, '0'));
                    clientStream.Write(buffer, 0, buffer.Length);
                    clientStream.Flush();
                    question_time--;
                }

                time_results--;

                if (time_results == 0)
                {
                    time_results = 2;
                    isCorrect = -1;
                    seconds_pass = 0;
                    question_time = question_time_const;
                    isUserAnswered = false;
                    answer_number = "5";

                    ans1_button.Background = Brushes.White;
                    ans2_button.Background = Brushes.White;
                    ans3_button.Background = Brushes.White;
                    ans4_button.Background = Brushes.White;

                    question_label.Content = question;
                    ans1_button.Content = answer1;
                    ans2_button.Content = answer2;
                    ans3_button.Content = answer3;
                    ans4_button.Content = answer4;
                }
            }

        }

        private void Leave_Game(object sender, RoutedEventArgs e)
        {
            byte[] buffer = new ASCIIEncoding().GetBytes("222");
            clientStream.Write(buffer, 0, buffer.Length);
            clientStream.Flush();
        }

        private void answer_click(object sender, RoutedEventArgs e)
        {
            Button button = sender as Button;

            isUserAnswered = true;
            answer_number = "" + button.Name[3];

            ans1_button.Background = Brushes.White;
            ans2_button.Background = Brushes.White;
            ans3_button.Background = Brushes.White;
            ans4_button.Background = Brushes.White;

            button.Background = Brushes.Gray;
        }

        private void next_question()
        {
            while (true)
            {
                this.Dispatcher.Invoke(() =>
                {
                    if (isCorrect == 0)
                    {
                        if (answer_number == "1")
                        {
                            ans1_button.Background = Brushes.Red;
                        }
                        else if (answer_number == "2")
                        {
                            ans2_button.Background = Brushes.Red;
                        }
                        else if (answer_number == "3")
                        {
                            ans3_button.Background = Brushes.Red;
                        }
                        else if (answer_number == "4")
                        {
                            ans4_button.Background = Brushes.Red;
                        }
                        else if (answer_number == "5")
                        {
                            ans1_button.Background = Brushes.Red;
                            ans2_button.Background = Brushes.Red;
                            ans3_button.Background = Brushes.Red;
                            ans4_button.Background = Brushes.Red;
                        }
                    }
                    else if (isCorrect == 1)
                    {
                        if (answer_number == "1")
                        {
                            ans1_button.Background = Brushes.Green;
                        }
                        else if (answer_number == "2")
                        {
                            ans2_button.Background = Brushes.Green;
                        }
                        else if (answer_number == "3")
                        {
                            ans3_button.Background = Brushes.Green;
                        }
                        else if (answer_number == "4")
                        {
                            ans4_button.Background = Brushes.Green;
                        }
                    }
                });

                byte[] buffer = new byte[4096];
                int bytesRead = clientStream.Read(buffer, 0, 4096);

                string server_answer = Encoding.ASCII.GetString(buffer.ToArray(), 0, (int)buffer.Length);

                this.Dispatcher.Invoke(() =>
                {
                    if (server_answer.Substring(0, 3) == "120")
                    {
                        if (server_answer[3] == '1')
                        {
                            num_correct_answers++;
                            isCorrect = 1;
                        }
                        else
                        {
                            isCorrect = 0;
                        }

                    }
                    else if (server_answer.Substring(0, 3) == "118")
                    {
                        server_answer = server_answer.Substring(3);

                        int question_size = int.Parse(server_answer.Substring(0, 3));
                        server_answer = server_answer.Substring(3);
                        question = server_answer.Substring(0, question_size);
                        server_answer = server_answer.Substring(question_size);

                        int answer1_size = int.Parse(server_answer.Substring(0, 3));
                        server_answer = server_answer.Substring(3);
                        answer1 = server_answer.Substring(0, answer1_size);
                        server_answer = server_answer.Substring(answer1_size);

                        int answer2_size = int.Parse(server_answer.Substring(0, 3));
                        server_answer = server_answer.Substring(3);
                        answer2 = server_answer.Substring(0, answer2_size);
                        server_answer = server_answer.Substring(answer2_size);

                        int answer3_size = int.Parse(server_answer.Substring(0, 3));
                        server_answer = server_answer.Substring(3);
                        answer3 = server_answer.Substring(0, answer3_size);
                        server_answer = server_answer.Substring(answer3_size);

                        int answer4_size = int.Parse(server_answer.Substring(0, 3));
                        server_answer = server_answer.Substring(3);
                        answer4 = server_answer.Substring(0, answer4_size);
                        server_answer = server_answer.Substring(answer4_size);
                    }
                    else if (server_answer.Substring(0, 3) == "121")
                    {
                        Thread.Sleep(1000);
                        _timer.Stop();
                        Game_Results game_results = new Game_Results(username, clientStream, server_answer.Substring(3));
                        this.Close();
                        game_results.Show();
                        thread.Abort();
                    }
                    else if (server_answer.Substring(0, 3) == "122")
                    {
                        _timer.Stop();
                        Menu menu = new Menu(username, clientStream);
                        this.Close();
                        menu.Show();
                        thread.Abort();
                    }
                });
            }
        }
    }
}
