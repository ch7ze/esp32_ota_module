import socket
import sys
import threading
import msvcrt

# Konfiguriere die UDP-Verbindung
UDP_IP_PC = "192.168.43.23"  # IP-Adresse des PCs
UDP_IP_ESP = "192.168.43.156"  # IP-Adresse des ESP32
UDP_PORT = 12345

# Socket für das Empfangen von Nachrichten
recv_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
recv_sock.bind((UDP_IP_PC, UDP_PORT))

# Socket für das Senden von Nachrichten
send_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def get_key():
    return msvcrt.getch().decode('utf-8')

def receive_udp():
    print(f"Listening for UDP packets on port {UDP_PORT}...")
    while True:
        data, addr = recv_sock.recvfrom(1024)  # Puffergröße ist 1024 Bytes
        try: 
            print(data.decode('utf-8'), end="")  # Gebe die empfangene Nachricht aus
        except UnicodeDecodeError:
            pass

def send_udp():
    print("Press WASD to control the ESP32. Press 'q' to quit.")
    while True:
        key = get_key()
        if key == 'q':
            break
        elif key in ['w', 'a', 's', 'd']:
            send_sock.sendto(key.encode('utf-8'), (UDP_IP_ESP, UDP_PORT))

# Starte den Empfangs-Thread
recv_thread = threading.Thread(target=receive_udp)
recv_thread.daemon = True
recv_thread.start()

# Starte die Sende-Funktion
send_udp()