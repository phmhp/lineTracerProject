#include <IRremote.h>

#define PIN_IR_RECEIVE 2

#define PIN_MOTOR_LEFT_FORWARD 10      // 왼쪽모터 전진
#define PIN_MOTOR_LEFT_BACKWARD 9      // 왼쪽모터 후진
#define PIN_MOTOR_RIGHT_FORWARD 6      // 오른쪽모터 전진
#define PIN_MOTOR_RIGHT_BACKWARD 5     // 오른쪽모터 후진

#define PIN_LINE_DETECT_CENTER A1      // 라인센서
#define REF_VALUE_LINE_DETECT 330      // 검은색 기준값
#define REF_VALUE_BACKGROUND_DETECT 80 // 흰색 기준값

int motorSpeedLeft = 0;
int motorSpeedRight = 0;
int isStartLineTracing = 0;
int detectValueCenter = 0;

void setup()
{
  IrReceiver.begin(PIN_IR_RECEIVE, ENABLE_LED_FEEDBACK); // IR리모컨 통신 시작
  pinMode(PIN_MOTOR_LEFT_FORWARD, OUTPUT);
  pinMode(PIN_MOTOR_LEFT_BACKWARD, OUTPUT);
  pinMode(PIN_MOTOR_RIGHT_FORWARD, OUTPUT);
  pinMode(PIN_MOTOR_RIGHT_BACKWARD, OUTPUT);

  analogWrite(PIN_MOTOR_LEFT_FORWARD, 0);
  analogWrite(PIN_MOTOR_LEFT_BACKWARD, 0);
  analogWrite(PIN_MOTOR_RIGHT_FORWARD, 0);
  analogWrite(PIN_MOTOR_RIGHT_BACKWARD, 0);

  Serial.begin(115200);
}

void processMotor()
{
  if (motorSpeedLeft >= 0)
  {
    analogWrite(PIN_MOTOR_LEFT_FORWARD, motorSpeedLeft);
    analogWrite(PIN_MOTOR_LEFT_BACKWARD, 0);
  }
  else
  {
    analogWrite(PIN_MOTOR_LEFT_FORWARD, 0);
    analogWrite(PIN_MOTOR_LEFT_BACKWARD, -motorSpeedLeft);
  }

  if (motorSpeedRight >= 0)
  {
    analogWrite(PIN_MOTOR_RIGHT_FORWARD, motorSpeedRight);
    analogWrite(PIN_MOTOR_RIGHT_BACKWARD, 0);
  }
  else
  {
    analogWrite(PIN_MOTOR_RIGHT_FORWARD, 0);
    analogWrite(PIN_MOTOR_RIGHT_BACKWARD, -motorSpeedRight);
  }
}

void loop()
{
  if (IrReceiver.decode())
  {
    if (IrReceiver.decodedIRData.protocol == NEC)
    {
      if (IrReceiver.decodedIRData.command == 0x16)
      {
        isStartLineTracing = 1; // * 버튼: 시작
      }
      else if (IrReceiver.decodedIRData.command == 0x0D)
      {
        isStartLineTracing = 0; // # 버튼: 정지
      }
    }
    IrReceiver.resume();
  }

  if (isStartLineTracing == 0)
  {
    motorSpeedLeft = 0;
    motorSpeedRight = 0;
    processMotor();
    return;
  }

  detectValueCenter = analogRead(PIN_LINE_DETECT_CENTER);
  Serial.println(detectValueCenter);

  if (detectValueCenter >= REF_VALUE_LINE_DETECT)
  {
    // 검은색 감지 → 좌회전
    motorSpeedLeft = 80;
    motorSpeedRight = 110;
  }
  else if (detectValueCenter <= REF_VALUE_BACKGROUND_DETECT)
  {
    // 흰색 감지 → 우회전
    motorSpeedLeft = 110;
    motorSpeedRight = 60;
  }
  else
  {
    // 중간 경계 → 직진
    motorSpeedLeft = 110;
    motorSpeedRight = 110;
  }

  processMotor();
}
