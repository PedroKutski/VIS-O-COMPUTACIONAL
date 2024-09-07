import cv2
import serial
import time
import mediapipe as mp

# Configuração da comunicação serial
# Conecta ao Arduino na porta COM10 com uma taxa de transmissão de 9600 bps
arduino = serial.Serial('COM10', 9600)
time.sleep(2)  # Aguarda 2 segundos para garantir que a conexão esteja estabelecida

# Inicialização da webcam
# Abre a webcam para captura de vídeo
cap = cv2.VideoCapture(0)

# Inicialização do MediaPipe Hands
# Configura o MediaPipe para reconhecimento de mãos
mp_hands = mp.solutions.hands
hands = mp_hands.Hands()
mp_drawing = mp.solutions.drawing_utils

def count_fingers(hand_landmarks):
    # Define os índices dos pontos de ponta dos dedos
    finger_tips = [8, 12, 16, 20]
    count = 0
    for tip in finger_tips:
        # Verifica se a ponta do dedo está acima do ponto de articulação
        if hand_landmarks.landmark[tip].y < hand_landmarks.landmark[tip - 2].y:
            count += 1
    return count

while cap.isOpened():
    # Captura um frame da webcam
    ret, frame = cap.read()
    if not ret:
        break  # Sai do loop se não houver mais frames

    # Converte o frame para o formato RGB para o MediaPipe
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    # Processa o frame para detectar as mãos
    results = hands.process(frame_rgb)

    # Se forem detectadas mãos no frame
    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            # Desenha as conexões das mãos no frame
            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)
            # Conta o número de dedos levantados
            fingers_up = count_fingers(hand_landmarks)
            # Envia a contagem de dedos para o Arduino
            if fingers_up == 1:
                arduino.write(b'1')
            elif fingers_up == 2:
                arduino.write(b'2')
            elif fingers_up == 3:
                arduino.write(b'3')
            else:
                arduino.write(b'0')

    # Exibe o frame com as marcações das mãos
    cv2.imshow("feed", frame)
    # Encerra o loop se a tecla 'Esc' for pressionada
    if cv2.waitKey(10) == 27:
        break

# Libera os recursos e fecha as janelas quando o loop termina
cap.release()
cv2.destroyAllWindows()
