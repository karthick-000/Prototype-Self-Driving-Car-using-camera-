from flask import Flask, render_template, request, redirect, Response
import socket
import cv2

app = Flask(__name__)

ESP32_IP = "192.168.230.102"
ESP32_PORT = 1234

cap = cv2.VideoCapture(0)  # Open webcam

def send_command(command):
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect((ESP32_IP, ESP32_PORT))
            sock.sendall((command + "\n").encode())
            print(f"Sent: {command}")
    except Exception as e:
        print(f"Failed to send command: {e}")

def generate_frames():
    while True:
        success, frame = cap.read()
        if not success:
            break
        else:
            ret, buffer = cv2.imencode('.jpg', frame)
            frame = buffer.tobytes()

            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + frame + b'\r\n')

@app.route('/', methods=['GET', 'POST'])
def index():
    if request.method == 'POST':
        command = request.form['command']
        send_command(command)
        return redirect('/')
    return render_template('index.html')

@app.route('/video_feed')
def video_feed():
    return Response(generate_frames(),
                    mimetype='multipart/x-mixed-replace; boundary=frame')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
