//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////DEFINO LOS PINES PARA LOS ULTRASONIDOS /// //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

#define echoPin 6
#define trigPin 7
#define echo_trig 8   
//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////// DEFINOO LOS PINES DE  SALIDAS PARA EL DRIVER DE MOTORES L298 (en este caso) //////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define Motor_izq_1  11
#define Motor_izq_2  10
#define Motor_der_1  13
#define Motor_der_2  12
//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////DEFINO LAS SALIDAS DE PWM PARA LOS MOTORES //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define pwm_motor_izq 3
#define pwm_motor_der 5
//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////  nombres para distinguir los sensores ultrasonidos /////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
#define izq     0
#define cen_izq 1
#define cen_der 2
#define der     3
#define atras   4
//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////  Variables para el control PID    //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
float Promedio, error, Pid,error_Anterior, derivativo, integral;
float pwm;
float Numerador,Denominador;
//////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////  Variables para los motores ////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
long x=0;
int pwm_der, pwm_izq;
int valor_de_sensores[5]={0,0,0,0,0}; // Guardo los valores de todos los sensores que tenga en un array de 8bit 
int set_Point=2.5;                    //valor fijo. //mas adelante se puede apreciar por que 2.5 en "Promedio"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//*************************************************************************************************************//

void setup() {
 Serial.begin(9600);               //Activo la comunicacion serial
 pinMode(trigPin,OUTPUT);          // 
 pinMode(echoPin,INPUT);           //

//Configuro los pines como salidas
 pinMode(Motor_izq_1,OUTPUT);      //
 pinMode(Motor_izq_2,OUTPUT);      //
 pinMode(Motor_der_1,OUTPUT);      //
 pinMode(Motor_der_2,OUTPUT);      //

 pinMode(pwm_motor_izq,OUTPUT);
 pinMode(pwm_motor_der,OUTPUT);
}
//////////////////////////////////////////////////////
//PROGRAMA PRINCIPAL//
/////////////////////////////////////////////////////
void loop() {
    lectura_de_sensores();        //Leo lo sensores 
    control_pid(0.80,4,0.001);    //Esta funcion sirve para cuando el robot detecte al enemigo, siempre lo "persiga" de frente. (kd,kp,ki)
    avanzar();                     //Con esta funcion el robot siempre va para adelante.
    Imprimir_lecturas();       //Muestro en pantalla el valor de las variables (PID,PWM de los motores,sensores, etc).
}
////////////////////////////////////////////////////
///////////////////////////////////////////////////
//////////////////////////////////////////////////
void lectura_de_sensores(){  //
 if(x==0){ //Lectura de sensor ultrasonido (4pines) utilizo este para "swichear" los ultrasonidos, ya que sino hago esto el ultrasonido 4 pines, al leer el valor, me detecta cualquier cosa
      digitalWrite(echoPin,LOW);  
      digitalWrite(trigPin,LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin,HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin,LOW);
      valor_de_sensores[1] = pulseIn(echoPin,HIGH);
      valor_de_sensores[1]=valor_de_sensores[1]/58.2;
      x=1; 
}else{
    //////////////////////////////////////////////////////////////////
    ////////////////LECTURA DE SENSOR ULTRASONIDO 3 pines//////////// 
    ////////////////////////////////////////////////////////////////
    //PD: para el robot sumo voy a utilizar Ultrasonidos de 3 pines.
    pinMode(echo_trig,OUTPUT);          //Configo el pin como Salida
    digitalWrite(echo_trig,HIGH);       //Mando a nivel alto 
    delayMicroseconds(10);              //demora de 10ms
    digitalWrite(echo_trig,LOW);        //Mando a nivel bajo
    pinMode(echo_trig,INPUT);           //Configuro otra vez el pin, pero esta vez como Entrada
    int filtro= pulseIn(echo_trig,HIGH);//Guardo el valor en una variable llamada "filtro"
    if(filtro>0){                       //Utilizo "filtro" por que, cuando el ultrasonido no detecta ningun objeto, tiende a tirar valores negativo. Tambien para una futura funcion sirve.
    valor_de_sensores[2]=filtro;       //Si el valor obtenido es mayor a 0 lo guardo en valor_de_sensor en la posicion 2
    valor_de_sensores[2]=((float(valor_de_sensores[2]/1000.0))*34.32)/2; //convierto el valor obtenido en Cm
    x=0;
  }
  }
}

  void Imprimir_lecturas(){
    //Imprimo todos los valores que quiera y los muestro en pantalla.
    Serial.print(" SENSOR2 = ");
    Serial.print(valor_de_sensores[1]);
    Serial.print(" SENSOR3 = ");
    Serial.print(valor_de_sensores[2]);
    
    Serial.print(" PROMEDIO = ");
    Serial.print(Promedio);
    Serial.print(" ERROR = ");
    Serial.print (error);
  
   // Serial.print(" PID= ");
    //Serial.print(Pid);
    Serial.print(" PWM DEr= ");
    Serial.print(pwm_der);
    Serial.print(" PWM izq= ");
    Serial.print(pwm_izq);
    Serial.print('\r'); //retorno de carro
    Serial.print('\n'); // nueva linea
  }
  
  void control_pid(float Kp, float Kd, float Ki){  
     //Con esto lo que se intenta lograr es que cada vez que el robot detecte un objeto(en este caso seria el robot rival) siempre va a tratar de seguirlo de frente.
      Numerador=((1.0*valor_de_sensores[izq])+( 2.0*valor_de_sensores[cen_izq] )+( 3.0*valor_de_sensores[cen_der] )+( 4.0*valor_de_sensores[der] )); //Sumo y multiplicos los valores obtenidos de los sensores. 
      Denominador=(valor_de_sensores[izq]+valor_de_sensores[cen_izq]+valor_de_sensores[cen_der]+valor_de_sensores[der]); //sumo los valores obtenidos de los sensores
    
      Promedio= Numerador/Denominador;  //Promedio ponderado. Con este promedio se puede obtener el centro del robot, que en este caso seria 2.5 (Set_point);
      error=(Promedio-0.50)-set_Point;  //Calculo el error. Si el robot esta en el centro, ejemplo Los dos sensores de adelante estan en "50" y los de los costado en "0"
                                        //el valor de error=0. el 0.50 al simular esos valores me daba como resultado 0.50 y no 0.

      
      integral+=error_Anterior;//integral.
      derivativo=(error-error_Anterior);// Actualizo el margen de error
  
  //Pid=((Kp*error)+(Kd*(error-error_Anterior))+Ki);
    Pid= ((Kp*error)+(derivativo*Kd)+(Ki*integral));
    error_Anterior=error;              //Guardo el error como error anterior
 
  }
  void avanzar(){
    Avanzar_adelante();
    if(Pid>0){
    pwm=(Pid*50);
    pwm_izq=(200+pwm);
    analogWrite(pwm_motor_izq,0);
    analogWrite(pwm_motor_der,pwm_izq);
    }
    if(Pid<0){
      Pid=((-1)*Pid);
      pwm=(Pid*50);
      pwm_der=(200+pwm);
      analogWrite(pwm_motor_izq,pwm_der);
      analogWrite(pwm_motor_der,0);
      }
    }

 void Avanzar_adelante(){
    digitalWrite(Motor_izq_1,HIGH);
    digitalWrite(Motor_izq_2,LOW);
    digitalWrite(Motor_der_1,HIGH);
    digitalWrite(Motor_der_2,LOW);
  }
  void stop_motores(){
    digitalWrite(Motor_izq_1,LOW);
    digitalWrite(Motor_izq_2,LOW);
    digitalWrite(Motor_der_1,LOW);
    digitalWrite(Motor_der_2,LOW);
    analogWrite(pwm_motor_der,0);
    analogWrite(pwm_motor_izq,0);
  }
  
