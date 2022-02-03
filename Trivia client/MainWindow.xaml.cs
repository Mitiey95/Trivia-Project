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
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Trivia_client
{
    public partial class MainWindow : Window
    {
        bool sign_button_flag = true;
        //if true -> sign up
        //if false -> sign in

        String username;
        String password;
        String email;

        public MainWindow()
        {
            InitializeComponent();
            this.email_textBox.Visibility = Visibility.Hidden;
            this.email_label.Visibility = Visibility.Hidden;

        }

        private void Switch_sign(object sender, RoutedEventArgs e)
        {   
            if (sign_button_flag)
            {
                this.email_textBox.Visibility = Visibility.Visible;
                this.email_label.Visibility = Visibility.Visible;
                this.sign_button.Content = "Sign in";
                sign_button_flag = false;
            }
            else
            {
                this.email_textBox.Visibility = Visibility.Hidden;
                this.email_label.Visibility = Visibility.Hidden;
                this.sign_button.Content = "Sign up";
                sign_button_flag = true;
            }
        }

        private void Done(object sender, RoutedEventArgs e)
        {
            username = this.username_textBox.Text;
            password = this.password_textBox.Text;
            email = this.email_textBox.Text;

            TcpClient client = new TcpClient();
            IPEndPoint serverEndPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 8826);

            try
            {
                client.Connect(serverEndPoint);
                NetworkStream clientStream = client.GetStream();

                if (sign_button_flag)
                {
                    if (sign_in(client, clientStream, username, password))
                    {
                        Menu menu = new Menu(username, clientStream);
                        this.Close();
                        menu.Show();
                    }
                }
                else
                {
                    sign_up(client, clientStream, username, password, email);
                }

                //byte[] buffer = new ASCIIEncoding().GetBytes("299");        // EXIT
                //clientStream.Write(buffer, 0, buffer.Length);
                //clientStream.Flush();
            }
            catch
            {
                this.status_label.Content = "Connect to Server lose";
            }
        }
        private bool sign_in(TcpClient client, NetworkStream clientStream, String username, String password)
        {
            String username_length = username.Length.ToString().PadLeft(2, '0');

            String password_length = password.Length.ToString().PadLeft(2, '0');

            byte[] buffer = new ASCIIEncoding().GetBytes("200" + username_length + username + password_length + password);
            clientStream.Write(buffer, 0, buffer.Length);
            clientStream.Flush();

            buffer = new byte[4096];
            int bytesRead = clientStream.Read(buffer, 0, 4096);

            String server_answer = Encoding.ASCII.GetString(buffer.ToArray(), 0, (int)buffer.Length);

            if (server_answer[3] == '0')
            {
                this.status_label.Content = "SUCCESS";
                return true;
            }
            else if (server_answer[3] == '1')
            {
                this.status_label.Content = "WRONG DETAILS";
            }
            else if (server_answer[3] == '2')
            {
                this.status_label.Content = "ALREADY CONNECTED";
            }
            return false;
        }

        private bool sign_up(TcpClient client, NetworkStream clientStream, String username, String password, string email)
        {
            String username_length = username.Length.ToString();
            if (username_length.Length < 10) { username_length = "0" + username_length; }

            String password_length = password.Length.ToString();
            if (password_length.Length < 10) { password_length = "0" + password_length; }

            String email_length = email.Length.ToString();
            if (email_length.Length < 10) { email_length = "0" + email_length; }

            byte[] buffer = new ASCIIEncoding().GetBytes("203" + username_length + username + password_length + password + email_length + email);
            clientStream.Write(buffer, 0, buffer.Length);
            clientStream.Flush();

            buffer = new byte[4096];
            int bytesRead = clientStream.Read(buffer, 0, 4096);

            String server_answer = Encoding.ASCII.GetString(buffer.ToArray(), 0, (int)buffer.Length);

            if (server_answer[3] == '0')
            {
                this.status_label.Content = "SUCCESS";
                return true;
            }
            else if (server_answer[3] == '1')
            {
                this.status_label.Content = "PASS ILLEGAL";
            }
            else if (server_answer[3] == '2')
            {
                this.status_label.Content = "USER IS ALREADY EXISTS";
            }
            else if (server_answer[3] == '3')
            {
                this.status_label.Content = "USERNAME ILLEGAL";
            }
            return false;
        }

        
        private void TextBox_GotFocus(object sender, RoutedEventArgs e)
        {
            if (username_textBox.Text == "Username")
            {
                //username_textBox.Text = "";
            }
        }

        private void TextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            if (username_textBox.Text == "")
            {
                //username_textBox.Text = "Username";
            }
        }
        
    }
}
