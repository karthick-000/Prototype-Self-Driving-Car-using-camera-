import cv2
import socket
import time
from ultralytics import YOLO

ESP_IP = "192.168.230.102"  # Replace with your ESP32's IP address
ESP_PORT = 1234

# Load YOLOv8 model (ensure "yolov8n.pt" is available)
model = YOLO("yolov8n.pt")
cap = cv2.VideoCapture(0)

prev_command = None
CONFIDENCE_THRESHOLD = 0.5


def send_command(command):
    global prev_command
    if command != prev_command:
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((ESP_IP, ESP_PORT))
            sock.send((command + "\n").encode())
            sock.close()
            print(f"Sent command: {command}")
            prev_command = command
        except Exception as e:
            print(f"Connection Error: {e}")
            time.sleep(2)


while True:
    ret, frame = cap.read()
    if not ret:
        break

    results = model(frame)
    obstacle_left = obstacle_right = obstacle_center = False
    detected_objects = False

    for result in results:
        for box, conf, cls in zip(result.boxes.xyxy, result.boxes.conf, result.boxes.cls):
            x_min, y_min, x_max, y_max = map(int, box[:4])
            confidence = float(conf)
            label = model.names[int(cls)]

            if confidence < CONFIDENCE_THRESHOLD:
                continue

            detected_objects = True
            center_x = (x_min + x_max) // 2

            # Decide direction based on object position:
            if center_x < frame.shape[1] / 3:
                obstacle_left = True
            elif center_x > (2 * frame.shape[1]) / 3:
                obstacle_right = True
            else:
                obstacle_center = True

            cv2.rectangle(frame, (x_min, y_min), (x_max, y_max), (0, 255, 0), 2)
            cv2.putText(frame, f"{label} {confidence:.2f}", (x_min, y_min - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    # Define command based on detected obstacles:
    if obstacle_center:
        command = "STOP"
    elif obstacle_left:
        command = "RIGHT"
    elif obstacle_right:
        command = "LEFT"
    else:
        command = "FORWARD"

    if not detected_objects:
        print("⚠️ No objects detected!")

    send_command(command)

    cv2.putText(frame, f"Direction: {command}", (50, 50),
                cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
    cv2.imshow("Object Detection", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
