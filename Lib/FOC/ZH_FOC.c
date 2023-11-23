#include "main.h"
#include "tim.h"
#include "tim.h"
#include "math.h"
#include "ZH_FOC.h"
//FOC��
//�Ƹ翪Դ��ת������������
//����DengFOC�ϲ��Թ�

//��ʼ��������������
#define _constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
//�궨��ʵ�ֵ�һ��Լ������,��������һ��ֵ�ķ�Χ��
//������˵���ú궨�������Ϊ _constrain�������������� amt��low �� high���ֱ��ʾҪ���Ƶ�ֵ����Сֵ�����ֵ���ú궨���ʵ��ʹ������Ԫ����������� amt �Ƿ�С�� low ����� high���������е�������Сֵ�����߷���ԭֵ��
//���仰˵����� amt С�� low���򷵻� low����� amt ���� high���򷵻� high�����򷵻� amt��������_constrain(amt, low, high) �ͻὫ amt Լ���� [low, high] �ķ�Χ�ڡ�
float voltage_power_supply=12;
float shaft_angle=0,open_loop_timestamp=0;
float zero_electric_angle=0,Ualpha,Ubeta=0,Ua=0,Ub=0,Uc=0,dc_a=0,dc_b=0,dc_c=0;
#define PI 3.1415926535897932384626433832795
//��ʼ��
void MOTOR1_PWM_Init(void)
{
	//PWM��ʼ��
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3,0);
	
//	//ͨ��4����ADC����
//	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
//	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4,3900);
//	//����ADCע��ת��
//	HAL_ADCEx_InjectedStart_IT(&hadc1);
}

// ��Ƕ����
float _electricalAngle(float shaft_angle, int pole_pairs) {
  return (shaft_angle * pole_pairs);
}

// ��һ���Ƕȵ� [0,2PI]
float _normalizeAngle(float angle){
  float a = fmod(angle, 2*PI);   //ȡ������������ڹ�һ�����г�����ֵ�������֪
  return a >= 0 ? a : (a + 2*PI);  
  //��Ŀ���������ʽ��condition ? expr1 : expr2 
  //���У�condition ��Ҫ��ֵ���������ʽ����������������򷵻� expr1 ��ֵ�����򷵻� expr2 ��ֵ�����Խ���Ŀ�������Ϊ if-else ���ļ���ʽ��
  //fmod �����������ķ����������ͬ����ˣ��� angle ��ֵΪ����ʱ�������ķ��Ž��� _2PI �ķ����෴��Ҳ����˵����� angle ��ֵС�� 0 �� _2PI ��ֵΪ�������� fmod(angle, _2PI) ��������Ϊ������
  //���磬�� angle ��ֵΪ -PI/2��_2PI ��ֵΪ 2PI ʱ��fmod(angle, _2PI) ������һ������������������£�����ͨ������������������ _2PI �����Ƕȹ�һ���� [0, 2PI] �ķ�Χ�ڣ���ȷ���Ƕȵ�ֵʼ��Ϊ������
}


// ����PWM�����������
void setPwm(float Ua, float Ub, float Uc) {

  // ����ռ�ձ�
  // ����ռ�ձȴ�0��1
  dc_a = _constrain(Ua / voltage_power_supply, 0.0f , 1.0f );
  dc_b = _constrain(Ub / voltage_power_supply, 0.0f , 1.0f );
  dc_c = _constrain(Uc / voltage_power_supply, 0.0f , 1.0f );
	
	//printf("d: %f, %f, %f\n",dc_a*4200,dc_b*4200,dc_c*4200);
  //д��PWM��PWM 0 1 2 ͨ��
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,(int)(dc_a*4200));
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,(int)(dc_b*4200));
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3,(int)(dc_c*4200));
}

void setPhaseVoltage(float Uq,float Ud, float angle_el) {
  angle_el = _normalizeAngle(angle_el + zero_electric_angle);
  // ������任
  Ualpha =  -Uq*sin(angle_el); 
  Ubeta =   Uq*cos(angle_el); 

  // ��������任
  Ua = Ualpha + voltage_power_supply/2;
  Ub = (sqrt(3)*Ubeta-Ualpha)/2 + voltage_power_supply/2;
  Uc = (-Ualpha-sqrt(3)*Ubeta)/2 + voltage_power_supply/2;
  setPwm(Ua,Ub,Uc);
}


//�����ٶȺ���
float velocityOpenloop(float target_velocity){
  unsigned long now_us = (unsigned long)HAL_GetTick();  //��ȡ�ӿ���оƬ������΢���������ľ����� 4 ΢�롣 micros() ���ص���һ���޷��ų����ͣ�unsigned long����ֵ
  
  //���㵱ǰÿ��Loop������ʱ����
  float Ts = (now_us - open_loop_timestamp) * 1e-6f;

  //���� micros() �������ص�ʱ������ڴ�Լ 70 ����֮�����¿�ʼ����������70�������䵽0ʱ��TS������쳣�������Ҫ�������������ʱ����С�ڵ��������� 0.5 �룬��������Ϊһ����С��Ĭ��ֵ���� 1e-3f
  if(Ts <= 0 || Ts > 0.5f) Ts = 1e-3f;
  

  // ͨ������ʱ������Ŀ���ٶ���������Ҫת���Ļ�е�Ƕȣ��洢�� shaft_angle �����С��ڴ�֮ǰ������Ҫ����ǶȽ��й�һ������ȷ����ֵ�� 0 �� 2�� ֮�䡣
  shaft_angle = _normalizeAngle(shaft_angle + target_velocity*Ts);
  //��Ŀ���ٶ�Ϊ 10 rad/s Ϊ�������ʱ������ 1 �룬����ÿ��ѭ������Ҫ���� 10 * 1 = 10 ���ȵĽǶȱ仯��������ʹ���ת����Ŀ���ٶȡ�
  //���ʱ������ 0.1 �룬��ô��ÿ��ѭ������Ҫ���ӵĽǶȱ仯������ 10 * 0.1 = 1 ���ȣ�����ʵ����ͬ��Ŀ���ٶȡ���ˣ�������ת���Ƕ�ȡ����Ŀ���ٶȺ�ʱ�����ĳ˻���

  // ʹ����ǰ���õ�voltage_power_supply��1/3��ΪUqֵ�����ֵ��ֱ��Ӱ���������
  // ���ֻ������ΪUq = voltage_power_supply/2������ua,ub,uc�ᳬ�������ѹ�޷�
  float Uq = voltage_power_supply/3;
  
  setPhaseVoltage(Uq,0, _electricalAngle(shaft_angle, 11));
  
  open_loop_timestamp = now_us;  //���ڼ�����һ��ʱ����

  return Uq;
}
