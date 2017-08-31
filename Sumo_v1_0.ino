
#define pulsador_start 13
#define led_status 12
///////////////////////////////////////////////////////////////////////////////
#define pwm_m_der 11
#define pwm_m_izq 10
#define giro_m_izq 9
#define giro_m_der 8
///////////////////////////////////////////////////////////////////////////////
#define ultrasonido_der 3
#define ultrasonido_centro_der 2
#define ultrasonido_centro_izq 1
#define ultrasonido_izq 0
////////////////////////////////////////////////////////////////////////////////
#define pin_sensor_cny_adelante_izq A0
#define pin_sensor_cny_atras_izq A1
#define pin_sensor_cny_atras_der A2
#define pin_sensor_cny_adelante_der A3
////////////////////////////////////////////////////////////////////////////////
int velocidad_ataque=255;
int velocidad_busqueda=200;
int borde_blanco = 500;
int distancia=50;
////////////////////////////////////////////////////////////////////////////////
int sensor_adelante_izq;
int sensor_adelante_der;
int sensor_atras_der;
int sensor_atras_izq;
////////////////////////////////////////////////////////////////////////////////
boolean bor_atras=false;
boolean bor_adelante=true;
boolean start=false;
////////////////////////////////////////////////////////////////////////////////
int trig[4]={1,2,5,7};
int echo[4]={0,3,4,6};
long distancia_sensor;
long tiempo;
int sensor[4]={0,0,0,0};
int puntero;
////////////////////////////////////////////////////////////////////////////////
boolean flag_atacar=false;
////////////////////////////////////////////////////////////////////////////////
/////////////////////////Programa Principal ///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void setup()
{
 // Serial.begin(9600);  
  Configuracion_GPIO();
  Pulsador_start();
}


void loop()
{
  Lectura_de_sensor_ultrasonidos();
  if(flag_atacar==true)  Atacar();
  else Busqueda();  
}
void Configuracion_GPIO(){
//Configuro los pines de I/O de los sensores ultrasonidos.
  pinMode(trig[0],OUTPUT);
  pinMode(echo[0],INPUT);
  pinMode(trig[1],OUTPUT);
  pinMode(echo[1],INPUT);
  pinMode(trig[2],OUTPUT);
  pinMode(echo[2],INPUT);
  pinMode(trig[3],OUTPUT);
  pinMode(echo[3],INPUT);
  ////////////////////////////////////////////////////
//Configuro los pines analogico como entrada para la lectura de los sensores CNY70  
  pinMode(pin_sensor_cny_adelante_izq,INPUT);
  pinMode(pin_sensor_cny_adelante_der,INPUT);
  pinMode(pin_sensor_cny_atras_der,INPUT);
  pinMode(pin_sensor_cny_atras_izq,INPUT); 
  ////////////////////////////////////////////////////
//Configuro las salidas de los motores
  pinMode(giro_m_der,OUTPUT);
  pinMode(giro_m_izq,OUTPUT);
  pinMode(pwm_m_der,OUTPUT);
  pinMode(pwm_m_izq,OUTPUT);
// Pines adicionales 
  pinMode(led_status,OUTPUT);
  pinMode(pulsador_start,INPUT);
}
///////////////////////////////////////////////////////////////////////////////
void Mostrar_pantalla_ultrasonidos()
{
  Serial.print(sensor[1]);
  Serial.print(" ");
  Serial.print(sensor[2]);
  Serial.print(" ");
  Serial.print(sensor[3]);
  Serial.println(" "); 
}
///////////////////////////////////////////////////////////////////////////////
void Mostrar_pantalla_opticos()
{
  Serial.print(1024-analogRead(A0));
  Serial.print(" ");
  Serial.print(1024-analogRead(A1));
  Serial.print(" ");
  Serial.print(1024-analogRead(A2));
  Serial.print(" ");
  Serial.println(1024-analogRead(A3));
  
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////****** ATAQUE *******////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Ultrasonido(int puntero)
{
  long valor_de_sensores;
  digitalWrite(echo[puntero],LOW);  
  digitalWrite(trig[puntero],LOW);
  delayMicroseconds(5);
  digitalWrite(trig[puntero],HIGH);
  delayMicroseconds(10);
  digitalWrite(trig[puntero],LOW);
  valor_de_sensores = pulseIn(echo[puntero],HIGH);
  sensor[puntero]=valor_de_sensores/58.2; 
}
////////////////////////////////////////////////////////////////////////////////
void Lectura_de_sensor_ultrasonidos()
{
  for(puntero=0;puntero<4;puntero++) Ultrasonido(puntero);
  if((sensor[0]<=distancia) || (sensor[1]<=distancia)|| (sensor[2]<=distancia)|| (sensor[3]<=distancia)) flag_atacar=true;  // Si detecto algo pongo al robot en modo ataque
}

void Avanzar_atacar()
{
  digitalWrite(giro_m_der,LOW);
  digitalWrite(giro_m_izq,LOW);
  analogWrite(pwm_m_der,velocidad_ataque);
  analogWrite(pwm_m_izq,velocidad_ataque);
}
void Giro_der_ataque()
{
 digitalWrite(giro_m_der,HIGH);
 digitalWrite(giro_m_izq,LOW);
 analogWrite(pwm_m_der,velocidad_ataque);
 analogWrite(pwm_m_izq,velocidad_ataque);
}
void Giro_izq_ataque()
{
  digitalWrite(giro_m_izq,HIGH);
  digitalWrite(giro_m_der,LOW);
  analogWrite(pwm_m_der,velocidad_ataque);
  analogWrite(pwm_m_izq,velocidad_ataque);
}
void Movimiento_off()
{
  digitalWrite(giro_m_der,LOW);
  digitalWrite(giro_m_izq,LOW);
  analogWrite(pwm_m_der,0);
  analogWrite(pwm_m_izq,0);
}
////////////////////////////////////////////////////////////////////////////////
void Atacar()
{
  if( (sensor[0]>distancia) && (sensor[1]>distancia) && (sensor[2]>distancia) && (sensor[3]>distancia) ) flag_atacar=false; //Si no detecto nada apago el modo "ataque"
  else
  {
    if( (sensor[ultrasonido_centro_der]<=distancia) && (sensor[ultrasonido_centro_izq]<=distancia) ) Avanzar_atacar();
    if( sensor[ultrasonido_der]<=distancia ) Giro_izq_ataque();
    if( sensor[ultrasonido_izq]<=distancia ) Giro_der_ataque();
  }
}
////////////////////////////////////////////////////////////////////////////////
///////////////////////******* BUSQUEDA ******//////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void Avanzar_busqueda()
{
  digitalWrite(giro_m_der,LOW);
  digitalWrite(giro_m_izq,LOW);
  analogWrite(pwm_m_der,velocidad_busqueda);
  analogWrite(pwm_m_izq,velocidad_busqueda);
}
////////////////////////////////////////////////////////////////////////////////
void Retroceder_busqueda()
{
  digitalWrite(giro_m_der,HIGH);
  digitalWrite(giro_m_izq,HIGH);
  analogWrite(pwm_m_der,255);
  analogWrite(pwm_m_izq,255);
  delay(500);  
}
////////////////////////////////////////////////////////////////////////////////
void Busqueda()
{
  Lectura_de_sensores_opticos();
  Rebotar();  
}
////////////////////////////////////////////////////////////////////////////////
void Lectura_de_sensores_opticos()
{
  sensor_adelante_izq=1023-analogRead(pin_sensor_cny_adelante_izq);
  sensor_adelante_der=1023-analogRead(pin_sensor_cny_adelante_der);
  sensor_atras_izq=1023-analogRead(pin_sensor_cny_atras_izq);
  sensor_atras_der=1023-analogRead(pin_sensor_cny_atras_der);  
}
////////////////////////////////////////////////////////////////////////////////
void Rebotar()
{
  if((sensor_adelante_izq>=borde_blanco) || (sensor_adelante_der>=borde_blanco))
  {
    bor_atras=false;
    bor_adelante=true;
    Retroceder_busqueda();
    Girar_atras_busqueda();    
  }
 /* if((sensor_atras_der>=borde_blanco) || (sensor_atras_izq>=borde_blanco)) 
  {
    bor_adelante=false;
    bor_atras=true;
  }*/
  if(bor_adelante==true) Avanzar_busqueda();     
  //if(bor_atras==true) Avanzar_busqueda();
}
////////////////////////////////////////////////////////////////////////////////
void Girar_atras_busqueda()
{
  digitalWrite(giro_m_der,HIGH);
  digitalWrite(giro_m_izq,LOW);
  analogWrite(pwm_m_der,255);
  analogWrite(pwm_m_izq,255);
  delay(250);
}
////////////////////////////////////////////////////////////////////////////////  
void Pulsador_start()
{
  digitalWrite(led_status,HIGH);
  while(!start)
  {
    boolean flag_pulsador;
    if(digitalRead(pulsador_start)==LOW) flag_pulsador=true;
    while(flag_pulsador==true)
    {
      if(digitalRead(pulsador_start)==LOW) delay(100);
      else
      {
        flag_pulsador=false;
        start=true;
      }
    }
  }
   delay(5000);
   digitalWrite(led_status,LOW);
}
///////////////////////////// FIN DEL PROGRAMA /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
