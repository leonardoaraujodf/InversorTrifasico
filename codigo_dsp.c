/*********************************************************************************
// This code is created by SimCoder Version 9.1 for TI F28335 Hardware Target
//
// SimCoder is copyright by Powersim Inc., 2009-2011
//
// Date: March 03, 2017 16:55:26
**********************************************************************************/
#include	<math.h>
#include	"PS_bios.h"
typedef float DefaultType;
#define	GetCurTime() PS_GetSysTimer()

interrupt void Task();

// IN�CIO DA DECLARA��O DE VARI�VEIS //
float vLCA_A;								// tens�es instant�neas de fase no lado de corrente alternada (ponto de acoplamento);
float vLCA_B;
float vLCA_C;
float iLCA_A;								// correntes instant�neas de linha no lado de corrente alternada (ponto de acoplamento);
float iLCA_B;
float iLCA_C;
float vLCC;									// tens�o do lado de corrente cont�nua da unidade inversora;
int DIGITIN1;								// entrada digital (inicia conex�o do inversor);
int DIGITIN2;								// entrada digital (ou flutua ou despacha pot�ncia);
int DIGITIN3;								// entrada digital (inicia mitiga��o harm�nica);
float VPACN = 127;							// tens�o de fase de refer�ncia no lado de corrente alternada, volt-rms;
float VPACMI = 0.7;							// tens�o m�nima do lado de corrente alternada, em pu;
float IPACN = 8;							// corrente de refer�ncia no lado de corrente alternada, amp�re-rms;
float IPACMA = 15;							// corrente m�xima no lado de corrente alternada, amp�re-rms;
float FRERED = 60;							// frequ�ncia nominal da rede el�trica, em hertz;
float VCCNOM = 400;							// tens�o de refer�ncia (nominal) no lado de corrente cont�nua, em volt;
float VCCMAX = 700;							// tens�o de refer�ncia (nominal) no lado de corrente cont�nua, em volt;
float VCCMIN = 360;							// tens�o m�nima do lado de corrente cont�nua para opera��o da unidade inversora, em volt;
float FLAG1 = 0;							// modo de opera��o do conversor, quanto ao despacho de pot�ncia reativa (FLAG1 = 1 -> controle DROOP, FLAG1 = 2 -> fator de pot�ncia fixo determinado por FLAG2, FLAG1 = 0 -> n�o fornece ou absorve pot�ncia reativa);
float FLAG2 = 0.95;							// fator de pot�ncia de opera��o (para FLAG1 = 2);
float FREAQU = 20000;						// frequ�ncia com a qual o "microcontrolador" atualiza os valores de entrada, ou frequ�ncia do ADC (mesma frequ�ncia com que o loop do controle � lido), em hertz;
//float Lf = 0.0076624;						// indut�ncia total do filtro de conex�o, em henry;
float Lf = 0.0039666;						// indut�ncia total do filtro de conex�o, em henry;
float PI = 3.1415926536;					// constante;

float AJUSTE_VA = 1.499314;
float AJUSTE_VB = 1.497173;
float AJUSTE_VC = 1.497673;
float AJUSTE_IA = 1.497353;
float AJUSTE_IB = 1.498965;
float AJUSTE_IC = 1.496452;
float AJUSTE_VCC = 1.533069;
float CONSTRANS_VA = 197.0978;
float CONSTRANS_VB = 198.9052;
float CONSTRANS_VC = 196.7201;
float CONSTRANS_IA = 16.13869;
float CONSTRANS_IB = 16.1185;
float CONSTRANS_IC = 16.18651;
float CONSTRANS_VCC = 1028.3453;

int FLAGAUX0 = 0;							// verifica��o de sincronia com a rede el�trica;
int FLAGAUX1 = 0;							// ou flutua ou despacha pot�ncia se igual � 1, todas as chaves abertas se igual � 0 (VIRA VARI�VEL DE ENTRADA NO DSP);
int FLAGAUX2 = 1;							// todas as chaves abertas se igual � 1;
int FLAGAUX3 = 0;							// verifica��o da tens�o de entrada do inversor (igual � 1 se tens�o estiver acima da m�nima);
int FLAGAUX4 = 0;							// prote��o contra poss�vel curto-circuito na entrada ou na sa�da do inversor (igual � 1 se corrente for maior que a m�xima ou se tens�o for menor que m�nima ou se a tens�o de entrada for alta);
int FLAGAUX5 = 1;							// igual � 0 para estado flutuante, igual � 1 para despacho de pot�ncia (VIRA VARI�VEL DE ENTRADA NO DSP);
int FLAGAUX6 = 0;							// desabilita mitiga��o harm�nica se necess�rio;
float AUX1 = 0;								// faz contagem para iniciar mitiga��o de cada harm�nica uma de cada vez;
float AUX2 = 0;								// vari�vel auxiliar para limita��o de pot�ncia;
float AUX3 = 0;								// vari�vel auxiliar para contagem de tempo;
float FLAGPROT1 = 0;						// flag para indidcar que prote��o foi acionada devido sobrecorrente;
float FLAGPROT2 = 0;						// flag para indidcar que prote��o foi acionada devido sobretens�o no link CC;
float FLAGPROT3 = 0;						// flag para indidcar que prote��o foi acionada devido subtens�o no lado CA;
float VA = 0;								// tens�es trif�sicas do lado de corrente alternada obtidas pelo ADC;
float VB = 0;
float VC = 0;
float IA = 0;								// correntes trif�sicas do lado de corrente alternada obtidas pelo ADC;
float IB = 0;
float IC = 0;
float VCC = 0;								// tens�o no lado de corrente cont�nua;
float VCCF = 0;								// VCC filtrada;
float VCCF_NMU = 0;							// VCCF[n-1];
float FCF_VCC = 100;						// frequ�ncia de corte do filtro de VCC, em rad/s;
float V_ALFA = 0;							// calculada a partir de vLCA (tens�es no ponto de acoplamento);
float V_BETA = 0;							// calculada a partir de vLCA (tens�es no ponto de acoplamento);
float V_MED1 = 0;							// m�dulo da tens�o calculada a partir de V_ALFA e de V_BETA;
float VD_P = 0;								// tens�o de sequ�ncia positiva de eixo direto;
float VQ_P = 0;								// tens�o de sequ�ncia positiva de eixo em quadratura;
float VD_N = 0;								// tens�o de sequ�ncia negativa de eixo direto;
float VQ_N = 0;								// tens�o de sequ�ncia negativa de eixo em quadratura;
float VD_P_DC = 0;							// termo constante da tens�o de sequ�ncia positiva de eixo direto;
float VQ_P_DC = 0;							// termo constante da tens�o de sequ�ncia positiva de eixo em quadratura;
float VD_N_DC = 0;							// termo constante da tens�o de sequ�ncia negativa de eixo direto;
float VQ_N_DC = 0;							// termo constante da tens�o de sequ�ncia negativa de eixo em quadratura;
float VQ_P_DC_NMU = 0;						// VQ_P_DC[n-1];
float VD_P_DCF = 0;							// VD_P_DC filtrado;
float VD_N_DCF = 0;							// VD_N_DC filtrado;
float VQ_P_DCF = 0;							// VD_N_DC filtrado;
float VQ_N_DCF = 0;							// VQ_N_DC filtrado;
float VD_P_DCF_NMU = 0;						// VD_P_DCF[n-1];
float VQ_P_DCF_NMU = 0;						// VQ_P_DCF[n-1];
float VD_N_DCF_NMU = 0;						// VD_N_DCF[n-1];
float VQ_N_DCF_NMU = 0;						// VQ_N_DCF[n-1];
float VQ_P_DC_PU = 0;						// VQ_P_DC em pu;
float VQ_P_DC_PU_NMU = 0;					// VQ_P_DC_PU[n-1];
float FCFPLL = 125.6637;					// frequ�ncia de corte do filtro passa baixa utilizado no PLL, em rad/s;
float kp_PLL = 46;							// constante do PI utilizado no PLL;
float ki_PLL = 0.0435;						// constante do PI utilizado no PLL;
float FREQ_PLL = 0;							// frequ�ncia calculada pelo PLL;
float FREQ_PLL_NMU = 0;						// FREQ_PLL[n-1];
float ANG_REF_PLL = 0;						// �ngulo de refer�ncia para a transforma��o vetorial obtido a partir do PLL;
float ANG_REF_PLL_NMU = 0;					// ANG_REF_PLL[n-1];
float ANG_REF = 0;							// �ngulo de refer�ncia;
float VD_MED = 0;							// tens�o de eixo direto medida;
float VQ_MED = 0;							// tens�o de eixo em quadratura medida;
float V_MED2 = 0;							// tens�o medida;
float VD_MEDF = 0;							// VD_MED filtrada;
float VQ_MEDF = 0;							// VQ_MED filtrada;
float VD_MEDF_NMU = 0;						// VD_MEDF[n-1];
float VQ_MEDF_NMU = 0;						// VQ_MEDF[n-1];
float FCF_VMED = 250;						// frequ�ncia de corte do filtro V_MED, em rad/s;
float V_MED2F = 0;							// tens�o medida filtrada;
float VD_PU = 0;							// tens�o de eixo direto medida em pu;
float VQ_PU = 0;							// tens�o de eixo em quadratura medida em pu;
float V_PU = 0;								// tens�o em pu;
float VD_PUF = 0;							// VD_PU filtrada;
float VQ_PUF = 0;							// VQ_PU filtrada;
float VD_PUF_NMU = 0;						// VD_PUF[n-1];
float VQ_PUF_NMU = 0;						// VQ_PUF[n-1];
float FCF_VPU = 250;						// frequ�ncia de corte do filtro V_PU, em rad/s;
float V_PUF = 0;							// tens�o em pu filtrada;
float ID_MED = 0;							// corrente de eixo direto medida;
float IQ_MED = 0;							// corrente de eixo em quadratura medida;
float I_MED = 0;							// corrente medida;
float ID_MEDF = 0;							// ID_MED filtrada;
float IQ_MEDF = 0;							// IQ_MED filtrada;
float ID_MEDF_NMU = 0;						// ID_MEDF[n-1];
float IQ_MEDF_NMU = 0;						// IQ_MEDF[n-1];
float FCF_IMED = 250;						// frequ�ncia de corte do filtro I_MED, em rad/s;
float I_MEDF = 0;							// corrente medida filtrada;
float ID_PU = 0;							// corrente de eixo direto medida em pu;
float IQ_PU = 0;							// corrente de eixo em quadratura medida em pu;
float I_PU = 0;								// corrente em pu;
float ID_PUF = 0;							// ID_PU filtrada;
float IQ_PUF = 0;							// IQ_PU filtrada;
float ID_PUF_NMU = 0;						// ID_PUF[n-1];
float IQ_PUF_NMU = 0;						// IQ_PUF[n-1];
float FCF_IPU = 5000;						// frequ�ncia de corte do filtro I_PU, em rad/s;
float I_PUF = 0;							// I_PU filtrada;
float IHA = 0;								// correntes harm�nicas instant�neas;
float IHB = 0;
float IHC = 0;
float FCF_IH = 10;							// frequ�ncia de corte dos filtros das correntes harm�nicas;
float ID2_MED = 0;							// corrente harm�nica de eixo direto de 2� ordem;
float IQ2_MED = 0;							// corrente harm�nica de eixo em quadratura de 2� ordem;
float ID2_MEDF = 0;							// ID2_MED filtrada;
float IQ2_MEDF = 0;							// IQ2_MED filtrada;
float ID2_MEDF_NMU = 0;						// ID2_MEDF[n-1];
float IQ2_MEDF_NMU = 0;						// IQ2_MEDF[n-1];
float I2_MEDF = 0;							// amplitude da harm�nica de 2� ordem;
float ID5_MED = 0;							// corrente harm�nica de eixo direto de 5� ordem;
float IQ5_MED = 0;							// corrente harm�nica de eixo em quadratura de 5� ordem;
float ID5_MEDF = 0;							// ID5_MED filtrada;
float IQ5_MEDF = 0;							// IQ5_MED filtrada;
float ID5_MEDF_NMU = 0;						// ID5_MEDF[n-1];
float IQ5_MEDF_NMU = 0;						// IQ5_MEDF[n-1];
float I5_MEDF = 0;							// amplitude da harm�nica de 5� ordem;
float ID7_MED = 0;							// corrente harm�nica de eixo direto de 7� ordem;
float IQ7_MED = 0;							// corrente harm�nica de eixo em quadratura de 7� ordem;
float ID7_MEDF = 0;							// ID7_MED filtrada;
float IQ7_MEDF = 0;							// IQ7_MED filtrada;
float ID7_MEDF_NMU = 0;						// ID7_MEDF[n-1];
float IQ7_MEDF_NMU = 0;						// IQ7_MEDF[n-1];
float I7_MEDF = 0;							// amplitude da harm�nica de 7� ordem;
float ID11_MED = 0;							// corrente harm�nica de eixo direto de 11� ordem;
float IQ11_MED = 0;							// corrente harm�nica de eixo em quadratura de 11� ordem;
float ID11_MEDF = 0;						// ID11_MED filtrada;
float IQ11_MEDF = 0;						// IQ11_MED filtrada;
float ID11_MEDF_NMU = 0;					// ID11_MEDF[n-1];
float IQ11_MEDF_NMU = 0;					// IQ11_MEDF[n-1];
float I11_MEDF = 0;							// amplitude da harm�nica de 11� ordem;
float ID13_MED = 0;							// corrente harm�nica de eixo direto de 13� ordem;
float IQ13_MED = 0;							// corrente harm�nica de eixo em quadratura de 13� ordem;
float ID13_MEDF = 0;						// ID13_MED filtrada;
float IQ13_MEDF = 0;						// IQ13_MED filtrada;
float ID13_MEDF_NMU = 0;					// ID13_MEDF[n-1];
float IQ13_MEDF_NMU = 0;					// IQ13_MEDF[n-1];
float I13_MEDF = 0;							// amplitude da harm�nica de 13� ordem;
float P_MED = 0;							// pot�ncia ativa medida;
float Q_MED = 0;							// pot�ncia reativa medida;
float P_MEDF = 0;							// P_MED filtrada;
float Q_MEDF = 0;							// Q_MED filtrada;
float P_MEDF_NMU = 0;						// P_MEDF[n-1];
float Q_MEDF_NMU = 0;						// Q_MEDF[n-1];
float FCF_PMED = 100;						// frequ�ncia de corte do filtro de P_MED, em rad/s;
float FCF_QMED = 100;						// frequ�ncia de corte do filtro de Q_MED, em rad/s;
float P_PU = 0;								// pot�ncia ativa em pu;
float Q_PU = 0;								// pot�ncia reativa em pu;
float P_PUF = 0;							// P_PU filtrada;
float Q_PUF = 0;							// Q_PU filtrada;
float P_PUF_NMU = 0;						// P_PUF[n-1];
float Q_PUF_NMU = 0;						// Q_PUF[n-1];
float FCF_PPU = 100;						// frequ�ncia de corte do filtro de P_PU, em rad/s;
float FCF_QPU = 100;						// frequ�ncia de corte do filtro de Q_PU, em rad/s;
float P_REF = 0;							// pot�ncia ativa de refer�ncia;
float Q_REF = 0;							// pot�ncia reativa de refer�ncia;
float V_MAX = 1.05;							// m�xima tens�o no ponto de acoplamento para absor��o de pot�ncia reativa, em pu;
float V_MIN = 0.95;							// m�nima tens�o no ponto de acoplamento para fornecimento de pot�ncia reativa, em pu;
float Q_MAXC = 0.2;							// m�xima pot�ncia reativa a ser fornecida pelo conversor, em pu;
float Q_MAXI = 0.2;							// m�xima pot�ncia reativa a ser absorvida pelo conversor, em pu;

float Kp1 = 0.88535704067013;				// contante proporcional PI-1;
float Ti1 = 1/83.2130219007307;				// contante integral PI-1;
float Kp2 = 0.765177752213601;				// contante proporcional PI-2;
float Ti2 = 1/82.1103159864179;				// contante integral PI-2;
float Kp3 = 0.10428469072073;
float Ti3 = 2/48.2306465329167;
float Kp4 = 0.10428469072073;
float Ti4 = 2/48.2306465329167;


float ID_REF1 = 0;							// corrente de refer�ncia de eixo direto;
float ID_REF1_NMU = 0;						// ID_REF1[n-1];
float WUP_D = 0;							// vari�vel anti-windup de eixo direto;
float ERRO_P = 0;							// erro de pot�ncia ativa;
float ERRO_P_NMU = 0;						// ERRO_P[n-1];
float IQ_REF1 = 0;							// corrente de refer�ncia de eixo em quadratura;
float IQ_REF1_NMU = 0;						// IQ_REF1[n-1];
float WUP_Q = 0;							// vari�vel anti-windup de eixo em quadratura;
float ERRO_Q = 0;							// erro de pot�ncia reativa;
float ERRO_Q_NMU = 0;						// ERRO_Q[n-1];
float ID_REF2 = 0;							// corrente de refer�ncia de eixo direto ap�s verifica��o de limites;
float IQ_REF2 = 0;							// corrente de refer�ncia de eixo em quadratura ap�s verifica��o de limites;
float ERRO_ID = 0;							// erro de corrente de eixo direto;
float ERRO_ID_NMU = 0;						// ERRO_ID[n-1];
float VD_REF = 0;							// tens�o de eixo direto de refer�ncia;
float ERRO_IQ = 0;							// erro de corrente de eixo em quadratura;
float ERRO_IQ_NMU = 0;						// ERRO_IQ[n-1];
float VQ_REF = 0;							// tens�o de eixo em quadratura de refer�ncia;

// para iniciar com estado flutuante (8 A);
float VDPI3 = -0.3313;								// tens�o de eixo direto de sa�da do PI-3;
float VDPI3_NMU = -0.3313;							// VDPI3[n-1];
float VQPI4 = 0.01727;								// tens�o de eixo em quadratura de sa�da do PI-4;
float VQPI4_NMU = 0.01727;							// VQPI4[n-1];

float Kp_PIH2 = 0.65869620722049;			// constante proporcional para controlador de mitiga��o da harm�nica de 2� ordem;
float Ti_PIH2 = 1/10.8104906595571;			// constante integral para controlador de mitiga��o da harm�nica de 2� ordem;
float Kp_PIH5 = 0.65869620722049;			// constante proporcional para controlador de mitiga��o da harm�nica de 5� ordem;
float Ti_PIH5 = 1/10.8104906595571;			// constante integral para controlador de mitiga��o da harm�nica de 5� ordem;
float Kp_PIH7 = 0.65869620722049;			// constante proporcional para controlador de mitiga��o da harm�nica de 7� ordem;
float Ti_PIH7 = 1/10.8104906595571;			// constante integral para controlador de mitiga��o da harm�nica de 7� ordem;
float Kp_PIH11 = 0.65869620722049;			// constante proporcional para controlador de mitiga��o da harm�nica de 11� ordem;
float Ti_PIH11 = 1/10.8104906595571;		// constante integral para controlador de mitiga��o da harm�nica de 11� ordem;
float Kp_PIH13 = 0.65869620722049;			// constante proporcional para controlador de mitiga��o da harm�nica de 13� ordem;
float Ti_PIH13 = 1/10.8104906595571;		// constante integral para controlador de mitiga��o da harm�nica de 13� ordem;

float ERRO_ID2 = 0;							// erro de corrente de eixo direto (harm�nica de 2� ordem);
float ERRO_IQ2 = 0;							// erro de corrente de eixo em quadratura (harm�nica de 2� ordem);
float ERRO_ID2_NMU = 0;						// ERRO_ID2[n-1];
float ERRO_IQ2_NMU = 0;						// ERRO_IQ2[n-1];
float VD2PI = 0;							// tens�o de eixo direto (2� ordem) a ser sintetizada na sa�da do inversor;
float VQ2PI = 0;							// tens�o de eixo em quadratura (2� ordem) a ser sintetizada na sa�da do inversor;
float VD2PI_NMU = 0;						// VD2PI[n-1];
float VQ2PI_NMU = 0;						// VQ2PI[n-1];
float ERRO_ID5 = 0;							// erro de corrente de eixo direto (harm�nica de 5� ordem);
float ERRO_IQ5 = 0;							// erro de corrente de eixo em quadratura (harm�nica de 5� ordem);
float ERRO_ID5_NMU = 0;						// ERRO_ID5[n-1];
float ERRO_IQ5_NMU = 0;						// ERRO_IQ5[n-1];
float VD5PI = 0;							// tens�o de eixo direto (5� ordem) a ser sintetizada na sa�da do inversor;
float VQ5PI = 0;							// tens�o de eixo em quadratura (5� ordem) a ser sintetizada na sa�da do inversor;
float VD5PI_NMU = 0;						// VD5PI[n-1];
float VQ5PI_NMU = 0;						// VQ5PI[n-1];
float ERRO_ID7 = 0;							// erro de corrente de eixo direto (harm�nica de 7� ordem);
float ERRO_IQ7 = 0;							// erro de corrente de eixo em quadratura (harm�nica de 7� ordem);
float ERRO_ID7_NMU = 0;						// ERRO_ID7[n-1];
float ERRO_IQ7_NMU = 0;						// ERRO_IQ7[n-1];
float VD7PI = 0;							// tens�o de eixo direto (7� ordem) a ser sintetizada na sa�da do inversor;
float VQ7PI = 0;							// tens�o de eixo em quadratura (7� ordem) a ser sintetizada na sa�da do inversor;
float VD7PI_NMU = 0;						// VD7PI[n-1];
float VQ7PI_NMU = 0;						// VQ7PI[n-1];
float ERRO_ID11 = 0;						// erro de corrente de eixo direto (harm�nica de 11� ordem);
float ERRO_IQ11 = 0;						// erro de corrente de eixo em quadratura (harm�nica de 11� ordem);
float ERRO_ID11_NMU = 0;					// ERRO_ID11[n-1];
float ERRO_IQ11_NMU = 0;					// ERRO_IQ11[n-1];
float VD11PI = 0;							// tens�o de eixo direto (11� ordem) a ser sintetizada na sa�da do inversor;
float VQ11PI = 0;							// tens�o de eixo em quadratura (11� ordem) a ser sintetizada na sa�da do inversor;
float VD11PI_NMU = 0;						// VD11PI[n-1];
float VQ11PI_NMU = 0;						// VQ11PI[n-1];
float ERRO_ID13 = 0;						// erro de corrente de eixo direto (harm�nica de 13� ordem);
float ERRO_IQ13 = 0;						// erro de corrente de eixo em quadratura (harm�nica de 13� ordem);
float ERRO_ID13_NMU = 0;					// ERRO_ID13[n-1];
float ERRO_IQ13_NMU = 0;					// ERRO_IQ13[n-1];
float VD13PI = 0;							// tens�o de eixo direto (13� ordem) a ser sintetizada na sa�da do inversor;
float VQ13PI = 0;							// tens�o de eixo em quadratura (13� ordem) a ser sintetizada na sa�da do inversor;
float VD13PI_NMU = 0;						// VD13PI[n-1];
float VQ13PI_NMU = 0;						// VQ13PI[n-1];
float vMOD1_A = 0;							// moduladoras para sinais de frequ�ncia fundamental;
float vMOD1_B = 0;
float vMOD1_C = 0;
float vMOD2_A = 0;							// moduladoras para sinais de 2� ordem;
float vMOD2_B = 0;
float vMOD2_C = 0;
float vMOD5_A = 0;							// moduladoras para sinais de 5� ordem;
float vMOD5_B = 0;
float vMOD5_C = 0;
float vMOD7_A = 0;							// moduladoras para sinais de 7� ordem;
float vMOD7_B = 0;
float vMOD7_C = 0;
float vMOD11_A = 0;							// moduladoras para sinais de 11� ordem;
float vMOD11_B = 0;
float vMOD11_C = 0;
float vMOD13_A = 0;							// moduladoras para sinais de 13� ordem;
float vMOD13_B = 0;
float vMOD13_C = 0;
float vMOD_A = 0;							// moduladoras finais;
float vMOD_B = 0;
float vMOD_C = 0;
float MODS_VA = 0;							// moduladoras para chaves superiores;
float MODS_VB = 0;
float MODS_VC = 0;
float MODI_VA = 0;							// moduladoras para chaves inferiores;
float MODI_VB = 0;
float MODI_VC = 0;
float INDMOD = 0;

// FIM DA DECLARA��O DE VARI�VEIS //

interrupt void Task_1();

DefaultType	fGblCCS = 0.0;
DefaultType	fGblCCS_1 = 0.0;
DefaultType	fGblCCS_2 = 0.0;
DefaultType	fGblCCS_3 = 0.0;
DefaultType	fGblCCS_4 = 0.0;
DefaultType	fGblCCS_5 = 0.0;


interrupt void Task()
{
	DefaultType fADC, fZOH19, fADC_1, fZOH18, fADC_2, fZOH21, fADC_3, fZOH20, fADC_4, fZOH23;
	DefaultType fADC_5, fZOH22, fADC_6, fZOH24, fTI_DIN1, fTI_DIN1_1, fTI_DIN1_2, fCCS_6;
	PS_EnableIntr();

	fADC = PS_GetDcAdc(0);
	fADC_1 = PS_GetDcAdc(1);
	fADC_2 = PS_GetDcAdc(2);
	fADC_3 = PS_GetDcAdc(3);
	fADC_4 = PS_GetDcAdc(4);
	fADC_5 = PS_GetDcAdc(5);
	fADC_6 = PS_GetDcAdc(6);
	fTI_DIN1 = (PS_GetDigitInA() & ((Uint32)1 << 17)) ? 1 : 0;
	fTI_DIN1_1 = (PS_GetDigitInA() & ((Uint32)1 << 19)) ? 1 : 0;
	fTI_DIN1_2 = (PS_GetDigitInA() & ((Uint32)1 << 21)) ? 1 : 0;
	fZOH19 = fADC;
	fZOH18 = fADC_1;
	fZOH21 = fADC_2;
	fZOH20 = fADC_3;
	fZOH23 = fADC_4;
	fZOH22 = fADC_5;
	fZOH24 = fADC_6; // Leitura de tens�o no barramento CC
	{

		

		// IN�CIO DA INICIALIZA��O DE VARI�VEIS //

		float TAQU = 1/FREAQU; // per�odo de aquis��o;

		float SNOMIN = 3*VPACN*IPACN; // pot�ncia aparente nominal do conversor, em VA;

		float XACL_PU = (2*PI*FRERED*Lf)/(VPACN*VPACN/(SNOMIN/3)); // reat�ncia de acoplamento, em pu; <---- Verificar valores de base: S em Pu ou em VA ??

		// FIM DA INICIALIZA��O DE VARI�VEIS //

		// IN�CIO EXEC //
		// leitura dos valores de tens�o e de corrente pelo ADC;
		vLCA_A = fZOH19;
		iLCA_A = fZOH18;
		vLCA_B = fZOH21;
		iLCA_B = fZOH20;
		vLCA_C = fZOH23;
		iLCA_C = fZOH22;
		vLCC = fZOH24; // Leitura de tens�o no barramento CC

		// leitura das entradas digitais para comandos;
		DIGITIN1 = fTI_DIN1; //             <----- O que s�o estas entradas digitais ???
		DIGITIN2 = fTI_DIN1_1; //             <----- O que s�o estas entradas digitais ???
		DIGITIN3 = fTI_DIN1_2; //             <----- O que s�o estas entradas digitais ???

		// transforma��o das tens�es e correntes de entrada em valores reais;
		VA = (vLCA_A - AJUSTE_VA)*CONSTRANS_VA;
		VB = (vLCA_B - AJUSTE_VB)*CONSTRANS_VB;
		VC = (vLCA_C - AJUSTE_VC)*CONSTRANS_VC;
		IA = (iLCA_A - AJUSTE_IA)*CONSTRANS_IA;
		IB = (iLCA_B - AJUSTE_IB)*CONSTRANS_IB;
		IC = (iLCA_C - AJUSTE_IC)*CONSTRANS_IC;
		VCC = (vLCC - AJUSTE_VCC)*CONSTRANS_VCC;
		VCC = 501;      //                  <--- Por que est� usando este valor ????

		// c�lculo e filtragem digital da tens�o no lado de corrente cont�nua;
		VCCF_NMU = VCCF;
		VCCF = (VCCF_NMU + VCC*TAQU*FCF_VCC)/(TAQU*FCF_VCC + 1);

		///                         <------ Come�o da implementa��o da PLL

		// in�cio sincroniza��o com a rede el�trica //
		V_ALFA = 0.666667*(VA - 0.5*VB - 0.5*VC);
		V_BETA = 0.666667*(0.866025*VB - 0.866025*VC);
		V_MED1 = sqrt(V_ALFA*V_ALFA + V_BETA*V_BETA);

		// c�lculo das vari�veis direta e em quadratura de sequ�ncia positiva e negativa;
		VD_P = V_ALFA*cos(ANG_REF_PLL) + V_BETA*sin(ANG_REF_PLL);
		VQ_P = -V_ALFA*sin(ANG_REF_PLL) + V_BETA*cos(ANG_REF_PLL);
		VD_N = V_ALFA*cos(ANG_REF_PLL) - V_BETA*sin(ANG_REF_PLL);
		VQ_N = V_ALFA*sin(ANG_REF_PLL) + V_BETA*cos(ANG_REF_PLL);

		// c�lula de desacoplamento;
		VQ_P_DC_NMU = VQ_P_DC;
		VD_P_DC = VD_P - VD_N_DCF*cos(2*ANG_REF_PLL) - VQ_N_DCF*sin(2*ANG_REF_PLL);
		VQ_P_DC = VQ_P + VD_N_DCF*sin(2*ANG_REF_PLL) - VQ_N_DCF*cos(2*ANG_REF_PLL);
		VD_N_DC = VD_N - VD_P_DCF*cos(2*ANG_REF_PLL) + VQ_P_DCF*sin(2*ANG_REF_PLL);
		VQ_N_DC = VQ_N - VD_P_DCF*sin(2*ANG_REF_PLL) - VQ_P_DCF*cos(2*ANG_REF_PLL);

		// filtragem dos sinais de sa�da, filtros passa-baixa;
		VD_P_DCF_NMU = VD_P_DCF;
		VQ_P_DCF_NMU = VQ_P_DCF;
		VD_N_DCF_NMU = VD_N_DCF;
		VQ_N_DCF_NMU = VQ_N_DCF;
		VD_P_DCF = (VD_P_DCF_NMU + VD_P_DC*TAQU*FCFPLL)/(TAQU*FCFPLL + 1);
		VQ_P_DCF = (VQ_P_DCF_NMU + VQ_P_DC*TAQU*FCFPLL)/(TAQU*FCFPLL + 1);
		VD_N_DCF = (VD_N_DCF_NMU + VD_N_DC*TAQU*FCFPLL)/(TAQU*FCFPLL + 1);
		VQ_N_DCF = (VQ_N_DCF_NMU + VQ_N_DC*TAQU*FCFPLL)/(TAQU*FCFPLL + 1);

		// c�lculo do �ngulo de refer�ncia;
		FREQ_PLL_NMU = FREQ_PLL;
		FREQ_PLL = FREQ_PLL_NMU - kp_PLL*VQ_P_DC_NMU + (kp_PLL + ki_PLL*TAQU)*VQ_P_DC;
		ANG_REF_PLL_NMU = ANG_REF_PLL;
		ANG_REF_PLL = ANG_REF_PLL_NMU + TAQU*FREQ_PLL;

		if (ANG_REF_PLL >= 6.283185)
		{
			ANG_REF_PLL = 0;
		}
		if (ANG_REF_PLL < 0)
		{
			ANG_REF_PLL = 0;
		}
		ANG_REF = ANG_REF_PLL;

		// verifica��o de sincronia com a rede;
		if (ANG_REF > 4.712389 || ANG_REF < 1.570796)
		{
			FLAGAUX0 = 1;
		}
		else
		{
			FLAGAUX0 = 0;
		}

		// fim da sincroniza��o com a rede el�trica //

		// in�cio transforma��o vetorial de tens�es e correntes e filtragem digital //
		// transforma��o vetorial das tens�es medidas (abc para dq0);
		VD_MED = 0.666667*(VA*cos(ANG_REF) + VB*cos(ANG_REF - 2.094395) + VC*cos(ANG_REF + 2.094395));
		VQ_MED = -0.666667*(VA*sin(ANG_REF) + VB*sin(ANG_REF - 2.094395) + VC*sin(ANG_REF + 2.094395));
		V_MED2 = sqrt(VD_MED*VD_MED + VQ_MED*VQ_MED);

		// filtragem dos sinais de tens�o dq, filtros passa-baixa de primeira ordem;
		VD_MEDF_NMU = VD_MEDF;
		VQ_MEDF_NMU = VQ_MEDF;
		VD_MEDF = (VD_MEDF_NMU + VD_MED*TAQU*FCF_VMED)/(TAQU*FCF_VMED + 1);
		VQ_MEDF = (VQ_MEDF_NMU + VQ_MED*TAQU*FCF_VMED)/(TAQU*FCF_VMED + 1);
		V_MED2F = sqrt(VD_MEDF*VD_MEDF + VQ_MEDF*VQ_MEDF);

		// c�lculo das tens�es em pu;
		VD_PU = VD_MED/(VPACN*sqrt(2));
		VQ_PU = VQ_MED/(VPACN*sqrt(2));
		V_PU = sqrt(VD_PU*VD_PU + VQ_PU*VQ_PU);

		// filtragem dos sinais de tens�o dq em pu, filtros passa-baixa de primeira ordem;
		VD_PUF_NMU = VD_PUF;
		VQ_PUF_NMU = VQ_PUF;
		VD_PUF = (VD_PUF_NMU + VD_PU*TAQU*FCF_VPU)/(TAQU*FCF_VPU + 1);
		VQ_PUF = (VQ_PUF_NMU + VQ_PU*TAQU*FCF_VPU)/(TAQU*FCF_VPU + 1);
		V_PUF = sqrt(VD_PUF*VD_PUF + VQ_PUF*VQ_PUF);

		// transforma��o vetorial das correntes medidas (abc para dq0);
		ID_MED = 0.666667*(IA*cos(ANG_REF) + IB*cos(ANG_REF - 2.094395) + IC*cos(ANG_REF + 2.094395));
		IQ_MED = -0.666667*(IA*sin(ANG_REF) + IB*sin(ANG_REF - 2.094395) + IC*sin(ANG_REF + 2.094395));
		I_MED = sqrt(ID_MED*ID_MED + IQ_MED*IQ_MED);

		// filtragem dos sinais de corrente dq, filtros passa-baixa de primeira ordem;
		ID_MEDF_NMU = ID_MEDF;
		IQ_MEDF_NMU = IQ_MEDF;
		ID_MEDF = (ID_MEDF_NMU + ID_MED*TAQU*FCF_IMED)/(TAQU*FCF_IMED + 1);
		IQ_MEDF = (IQ_MEDF_NMU + IQ_MED*TAQU*FCF_IMED)/(TAQU*FCF_IMED + 1);
		I_MEDF = sqrt(ID_MEDF*ID_MEDF + IQ_MEDF*IQ_MEDF);

		// c�lculo das correntes em pu;
		ID_PU = ID_MED/IPACN;
		IQ_PU = IQ_MED/IPACN;
		I_PU = sqrt(ID_PU*ID_PU + IQ_PU*IQ_PU);

		// filtragem dos sinais de corrente dq, filtros passa-baixa de primeira ordem;
		ID_PUF_NMU = ID_PUF;
		IQ_PUF_NMU = IQ_PUF;
		ID_PUF = (ID_PUF_NMU + ID_PU*TAQU*FCF_IPU)/(TAQU*FCF_IPU + 1);
		IQ_PUF = (IQ_PUF_NMU + IQ_PU*TAQU*FCF_IPU)/(TAQU*FCF_IPU + 1);
		I_PUF = sqrt(ID_PUF*ID_PUF + IQ_PUF*IQ_PUF);
		// fim da transforma��o vetorial de tens�es e correntes e filtragem digital //

		// in�cio do c�lculo das correntes harm�nicas //
		// elimina��o da componente fundamental da corrente;
		IHA = IA - (ID_MEDF*cos(ANG_REF) - IQ_MEDF*sin(ANG_REF));
		IHB = IB - (ID_MEDF*cos(ANG_REF - 2.094395) - IQ_MEDF*sin(ANG_REF - 2.094395));
		IHC = IC - (ID_MEDF*cos(ANG_REF + 2.094395) - IQ_MEDF*sin(ANG_REF + 2.094395));

		// 2� harm�nica;
		ID2_MED = 0.666667*(IHA*cos(-2*ANG_REF) + IHB*cos(-2*ANG_REF - 2.094395) + IHC*cos(-2*ANG_REF + 2.094395));
		IQ2_MED = -0.666667*(IHA*sin(-2*ANG_REF) + IHB*sin(-2*ANG_REF - 2.094395) + IHC*sin(-2*ANG_REF + 2.094395));
		ID2_MEDF_NMU = ID2_MEDF;
		IQ2_MEDF_NMU = IQ2_MEDF;
		ID2_MEDF = (ID2_MEDF_NMU + ID2_MED*TAQU*FCF_IH)/(TAQU*FCF_IH + 1);
		IQ2_MEDF = (IQ2_MEDF_NMU + IQ2_MED*TAQU*FCF_IH)/(TAQU*FCF_IH + 1);
		I2_MEDF = sqrt(ID2_MEDF*ID2_MEDF + IQ2_MEDF*IQ2_MEDF);

		// 5� harm�nica;
		ID5_MED = 0.666667*(IHA*cos(-5*ANG_REF) + IHB*cos(-5*ANG_REF - 2.094395) + IHC*cos(-5*ANG_REF + 2.094395));
		IQ5_MED = -0.666667*(IHA*sin(-5*ANG_REF) + IHB*sin(-5*ANG_REF - 2.094395) + IHC*sin(-5*ANG_REF + 2.094395));
		ID5_MEDF_NMU = ID5_MEDF;
		IQ5_MEDF_NMU = IQ5_MEDF;
		ID5_MEDF = (ID5_MEDF_NMU + ID5_MED*TAQU*FCF_IH)/(TAQU*FCF_IH + 1);
		IQ5_MEDF = (IQ5_MEDF_NMU + IQ5_MED*TAQU*FCF_IH)/(TAQU*FCF_IH + 1);
		I5_MEDF = sqrt(ID5_MEDF*ID5_MEDF + IQ5_MEDF*IQ5_MEDF);

		// 7� harm�nica;
		ID7_MED = 0.666667*(IHA*cos(7*ANG_REF) + IHB*cos(7*ANG_REF - 2.094395) + IHC*cos(7*ANG_REF + 2.094395));
		IQ7_MED = -0.666667*(IHA*sin(7*ANG_REF) + IHB*sin(7*ANG_REF - 2.094395) + IHC*sin(7*ANG_REF + 2.094395));
		ID7_MEDF_NMU = ID7_MEDF;
		IQ7_MEDF_NMU = IQ7_MEDF;
		ID7_MEDF = (ID7_MEDF_NMU + ID7_MED*TAQU*FCF_IH)/(TAQU*FCF_IH + 1);
		IQ7_MEDF = (IQ7_MEDF_NMU + IQ7_MED*TAQU*FCF_IH)/(TAQU*FCF_IH + 1);
		I7_MEDF = sqrt(ID7_MEDF*ID7_MEDF + IQ7_MEDF*IQ7_MEDF);

		// 11� harm�nica;
		ID11_MED = 0.666667*(IHA*cos(-11*ANG_REF) + IHB*cos(-11*ANG_REF - 2.094395) + IHC*cos(-11*ANG_REF + 2.094395));
		IQ11_MED = -0.666667*(IHA*sin(-11*ANG_REF) + IHB*sin(-11*ANG_REF - 2.094395) + IHC*sin(-11*ANG_REF + 2.094395));
		ID11_MEDF_NMU = ID11_MEDF;
		IQ11_MEDF_NMU = IQ11_MEDF;
		ID11_MEDF = (ID11_MEDF_NMU + ID11_MED*TAQU*FCF_IH)/(TAQU*FCF_IH + 1);
		IQ11_MEDF = (IQ11_MEDF_NMU + IQ11_MED*TAQU*FCF_IH)/(TAQU*FCF_IH + 1);
		I11_MEDF = sqrt(ID11_MEDF*ID11_MEDF + IQ11_MEDF*IQ11_MEDF);

		// 13� harm�nica;
		ID13_MED = 0.666667*(IHA*cos(13*ANG_REF) + IHB*cos(13*ANG_REF - 2.094395) + IHC*cos(13*ANG_REF + 2.094395));
		IQ13_MED = -0.666667*(IHA*sin(13*ANG_REF) + IHB*sin(13*ANG_REF - 2.094395) + IHC*sin(13*ANG_REF + 2.094395));
		ID13_MEDF_NMU = ID13_MEDF;
		IQ13_MEDF_NMU = IQ13_MEDF;
		ID13_MEDF = (ID13_MEDF_NMU + ID13_MED*TAQU*FCF_IH)/(TAQU*FCF_IH + 1);
		IQ13_MEDF = (IQ13_MEDF_NMU + IQ13_MED*TAQU*FCF_IH)/(TAQU*FCF_IH + 1);
		I13_MEDF = sqrt(ID13_MEDF*ID13_MEDF + IQ13_MEDF*IQ13_MEDF);
		// fim do c�lculo das correntes harm�nicas;

		// in�cio do c�lculo das pot�ncias ativa e reativa //
		// c�lculo das pot�ncias ativa e reativa trif�sicas medidas;
		P_MED = 1.5*(VD_MED*ID_MED + VQ_MED*IQ_MED);
		Q_MED = 1.5*(VQ_MED*ID_MED - VD_MED*IQ_MED);

		// filtragem das pot�ncias medidas;
		P_MEDF_NMU = P_MEDF;
		Q_MEDF_NMU = Q_MEDF;
		P_MEDF = (P_MEDF_NMU + P_MED*TAQU*FCF_PMED)/(TAQU*FCF_PMED + 1);
		Q_MEDF = (Q_MEDF_NMU + Q_MED*TAQU*FCF_QMED)/(TAQU*FCF_QMED + 1);

		// c�lculo das pot�ncias para pu;
		P_PU = P_MED/SNOMIN;
		Q_PU = Q_MED/SNOMIN;

		// filtragem das pot�ncias medidas;
		P_PUF_NMU = P_PUF;
		Q_PUF_NMU = Q_PUF;
		P_PUF = (P_PUF_NMU + P_PU*TAQU*FCF_PPU)/(TAQU*FCF_PPU + 1);
		Q_PUF = (Q_PUF_NMU + Q_PU*TAQU*FCF_QPU)/(TAQU*FCF_QPU + 1);
		// fim c�lculo das pot�ncias ativa e reativa //
		
		// in�cio do c�lculo dos valores de refer�ncia para as pot�ncias //

		if (DIGITIN1 == 1)
		{

			FLAGAUX1 = 1; // ou flutua ou despacha pot�ncia;

		}

		else
		{

			FLAGAUX1 = 0; // nem flutua nem despacha pot�ncia;

		}

		if (FLAGAUX1 == 1)
		{
			if (VCCF < VCCNOM && FLAGAUX3 == 0)
			{
				FLAGAUX2 = 1; // todas as chaves do inversor abertas;
			}
			else
			{
				FLAGAUX2 = 0; // j� pode entrar em estado flutuante ou despachar pot�ncia;
				if (VCCF < VCCMIN)
				{
					FLAGAUX3 = 0;
					FLAGAUX2 = 1; // se tens�o � reestabelecida pode flutuar ou despachar pot�ncia novamente (tirar a pr�xima linha se for o caso);
					// FLAGAUX4 = 1; // talvez houve curto, interromper todo o processo definitivamente;
				}
				else
				{
					FLAGAUX3 = 1;
				}

				if (fabs(IA) > IPACMA*sqrt(2) || fabs(IB) > IPACMA*sqrt(2) || fabs(IC) > IPACMA*sqrt(2)) // prote��o;
				{
					FLAGAUX4 = 1; // tem que abrir todas as chaves do inversor;
					FLAGPROT1 = 1;
				}

				if (VCCF > VCCMAX) // prote��o;
				{
					FLAGAUX4 = 1; // tem que abrir todas as chaves do inversor;
					FLAGPROT2 = 1;
				}

				if (V_PUF < 0.5) // prote��o;
				{
					FLAGAUX4 = 1; // tem que abrir todas as chaves do inversor;
					FLAGPROT3 = 1;
				}

				if (DIGITIN2 == 0)
				{

					FLAGAUX5 = 0; // entra em estado flutuante;

				}

				else
				{

					FLAGAUX5 = 1; // despacha pot�ncia;

				}

				if (VD_PUF < VPACMI || FLAGAUX2 == 1 || FLAGAUX5 == 0 || FLAGAUX4 == 1)
				{
					P_REF = 0;
					Q_REF = 0;
				}
				else
				{
					// determina��o do despacho de pot�ncia ativa;
					P_REF = P_REF + 0.000005; // rampa de incremento de P_REF (10 segundos);
					if (P_REF > 1)
					{
						P_REF = 1;
					}

					//P_REF = VCCF/VCCNOM;

					//P_REF = 1;

					// determina��o do despacho de pot�ncia reativa;
					if (FLAG1 == 1)
					{
						Q_REF = -1*((VD_PUF - 1)/((V_MAX - V_MIN)/(Q_MAXC + Q_MAXI)));
						if (VD_PUF < V_MIN)
						{
							Q_REF = Q_MAXC;
						}
						if (VD_PUF > V_MAX)
						{
							Q_REF = Q_MAXI;
						}
					}
					if (FLAG1 == 2)
					{
						Q_REF = P_PUF*tan(acos(FLAG2));
					}
					if (FLAG1 != 1 && FLAG1 != 2)
					{
						Q_REF = 0;
					}
				}

			}
			if (FLAGAUX4 == 1)
			{
				FLAGAUX2 = 1; // em caso de curto-circuito, interromper todo o processo definitivamente;
			}
			// fim do c�lculo dos valores de refer�ncia para as pot�ncias //

			// c�lculo da corrente de eixo direto e em quadratura de refer�ncia;
			if (FLAGAUX2 == 0) // despacha pot�ncia ou estado flutuante;
			{
				ERRO_P_NMU = ERRO_P;
				ERRO_Q_NMU = ERRO_Q;
				ERRO_P = (P_REF - P_PUF)*1 + WUP_D;
				ERRO_Q = (Q_REF - Q_PUF)*1 + WUP_Q;

				// controlador PI-1 (PARALELO);
				ID_REF1_NMU = ID_REF1;
				ID_REF1 = ID_REF1_NMU + (TAQU/(2*Ti1) + Kp1)*ERRO_P + (TAQU/(2*Ti1) - Kp1)*ERRO_P_NMU;
				// controlador PI-2 (PARALELO);
				IQ_REF1_NMU = IQ_REF1;
				IQ_REF1 = IQ_REF1_NMU + (TAQU/(2*Ti2) + Kp2)*ERRO_Q + (TAQU/(2*Ti2) - Kp2)*ERRO_Q_NMU;


				//ID_REF1 = sqrt(2);
				//IQ_REF1 = -IQ_PUF;

				// aplica��o do bloco "an�lise dos limites operacionais da unidade inversora";
				if ((ID_REF1*ID_REF1 + IQ_REF1*IQ_REF1) <= 2)
				{
					ID_REF2 = ID_REF1;
					IQ_REF2 = IQ_REF1;
				}
				else
				{
					if (abs(ID_REF1) > sqrt(2))
					{
						if (ID_REF1 > 0)
						{
							ID_REF2 = sqrt(2);
						}
						else
						{
							ID_REF2 = -sqrt(2);
						}
						IQ_REF2 = 0;
					}
					else
					{
						ID_REF2 = ID_REF1;
						AUX2 = fabs(2 - ID_REF2*ID_REF2);
						if (IQ_REF1 > 0)
						{
							IQ_REF2 = sqrt(AUX2);
						}
						else
						{
							IQ_REF2 = -1*sqrt(AUX2);
						}
					}
				}

				// malha anti-windup de eixo direto e de eixo em quadratura;
				WUP_D = ID_REF2 - ID_REF1;
				WUP_Q = IQ_REF2 - IQ_REF1;

				ERRO_ID_NMU = ERRO_ID;
				ERRO_IQ_NMU = ERRO_IQ;
				ERRO_ID = (ID_REF2 - ID_PUF)*1;
				ERRO_IQ = -1*(IQ_REF2 + IQ_PUF)*1;

				// controlador PI-3;
				VDPI3_NMU = VDPI3;
				VDPI3 = VDPI3_NMU + (TAQU/(2*Ti3) + Kp3)*ERRO_ID + (TAQU/(2*Ti3) - Kp3)*ERRO_ID_NMU;
				// controlador PI-2;
				VQPI4_NMU = VQPI4;
				VQPI4 = VQPI4_NMU + (TAQU/(2*Ti4) + Kp4)*ERRO_IQ + (TAQU/(2*Ti4) - Kp4)*ERRO_IQ_NMU;

				// desacoplamento;
				VD_REF = VDPI3 + VD_PUF - XACL_PU*IQ_PUF;
				VQ_REF = VQPI4 + VQ_PUF + XACL_PU*ID_PUF;
				INDMOD = sqrt(VD_REF*VD_REF + VQ_REF*VQ_REF);

				// transforma��o vetorial inversa das tens�es (dq0 para abc);
				vMOD1_A = VD_REF*cos(ANG_REF) - VQ_REF*sin(ANG_REF);
				vMOD1_B = VD_REF*cos(ANG_REF - 2.094395) - VQ_REF*sin(ANG_REF - 2.094395);
				vMOD1_C = VD_REF*cos(ANG_REF + 2.094395) - VQ_REF*sin(ANG_REF + 2.094395);

				if (DIGITIN3 == 1 && FLAGAUX6 == 0 && DIGITIN2 == 1)
				{

					AUX1 = AUX1 + 0.00005;
					if (AUX1 > (30/60))
					{
						AUX1 = 30/60;
					}

					// malha de controle para 2� harm�nica;
					if (AUX1 > 100)
					{
						ERRO_ID2_NMU = ERRO_ID2;
						ERRO_IQ2_NMU = ERRO_IQ2;
						ERRO_ID2 = 0 - ID2_MEDF;
						ERRO_IQ2 = 0 - IQ2_MEDF;

						VD2PI_NMU = VD2PI;
						VQ2PI_NMU = VQ2PI;
						VD2PI = VD2PI_NMU + (TAQU/(2*Ti_PIH2) + Kp_PIH2)*ERRO_ID2 + (TAQU/(2*Ti_PIH2) - Kp_PIH2)*ERRO_ID2_NMU;
						VQ2PI = VQ2PI_NMU + (TAQU/(2*Ti_PIH2) + Kp_PIH2)*ERRO_IQ2 + (TAQU/(2*Ti_PIH2) - Kp_PIH2)*ERRO_IQ2_NMU;

						vMOD2_A = VD2PI*cos(-2*ANG_REF) - VQ2PI*sin(-2*ANG_REF);
						vMOD2_B = VD2PI*cos(-2*ANG_REF - 2.094395) - VQ2PI*sin(-2*ANG_REF - 2.094395);
						vMOD2_C = VD2PI*cos(-2*ANG_REF + 2.094395) - VQ2PI*sin(-2*ANG_REF + 2.094395);
					}

		 			// malha de controle para 5� harm�nica;
					if (AUX1 > (100/60))
					{
						ERRO_ID5_NMU = ERRO_ID5;
						ERRO_IQ5_NMU = ERRO_IQ5;
						ERRO_ID5 = 0 - ID5_MEDF;
						ERRO_IQ5 = 0 - IQ5_MEDF;

						VD5PI_NMU = VD5PI;
						VQ5PI_NMU = VQ5PI;
						VD5PI = VD5PI_NMU + (TAQU/(2*Ti_PIH5) + Kp_PIH5)*ERRO_ID5 + (TAQU/(2*Ti_PIH5) - Kp_PIH5)*ERRO_ID5_NMU;
						VQ5PI = VQ5PI_NMU + (TAQU/(2*Ti_PIH5) + Kp_PIH5)*ERRO_IQ5 + (TAQU/(2*Ti_PIH5) - Kp_PIH5)*ERRO_IQ5_NMU;

						vMOD5_A = VD5PI*cos(-5*ANG_REF) - VQ5PI*sin(-5*ANG_REF);
						vMOD5_B = VD5PI*cos(-5*ANG_REF - 2.094395) - VQ5PI*sin(-5*ANG_REF - 2.094395);
						vMOD5_C = VD5PI*cos(-5*ANG_REF + 2.094395) - VQ5PI*sin(-5*ANG_REF + 2.094395);
					}

		 			// malha de controle para 7� harm�nica;
					if (AUX1 > (100/60))
					{
						ERRO_ID7_NMU = ERRO_ID7;
						ERRO_IQ7_NMU = ERRO_IQ7;
						ERRO_ID7 = 0 - ID7_MEDF;
						ERRO_IQ7 = 0 - IQ7_MEDF;

						VD7PI_NMU = VD7PI;
						VQ7PI_NMU = VQ7PI;
						VD7PI = VD7PI_NMU + (TAQU/(2*Ti_PIH7) + Kp_PIH7)*ERRO_ID7 + (TAQU/(2*Ti_PIH7) - Kp_PIH7)*ERRO_ID7_NMU;
						VQ7PI = VQ7PI_NMU + (TAQU/(2*Ti_PIH7) + Kp_PIH7)*ERRO_IQ7 + (TAQU/(2*Ti_PIH7) - Kp_PIH7)*ERRO_IQ7_NMU;

						vMOD7_A = VD7PI*cos(7*ANG_REF) - VQ7PI*sin(7*ANG_REF);
						vMOD7_B = VD7PI*cos(7*ANG_REF - 2.094395) - VQ7PI*sin(7*ANG_REF - 2.094395);
						vMOD7_C = VD7PI*cos(7*ANG_REF + 2.094395) - VQ7PI*sin(7*ANG_REF + 2.094395);
					}

		 			// malha de controle para 11� harm�nica;
					if (AUX1 > (100/60))
					{
						ERRO_ID11_NMU = ERRO_ID11;
						ERRO_IQ11_NMU = ERRO_IQ11;
						ERRO_ID11 = 0 + ID11_MEDF;
						ERRO_IQ11 = 0 + IQ11_MEDF;

						VD11PI_NMU = VD11PI;
						VQ11PI_NMU = VQ11PI;
						VD11PI = VD11PI_NMU + (TAQU/(2*Ti_PIH11) + Kp_PIH11)*ERRO_ID11 + (TAQU/(2*Ti_PIH11) - Kp_PIH11)*ERRO_ID11_NMU;
						VQ11PI = VQ11PI_NMU + (TAQU/(2*Ti_PIH11) + Kp_PIH11)*ERRO_IQ11 + (TAQU/(2*Ti_PIH11) - Kp_PIH11)*ERRO_IQ11_NMU;

						vMOD11_A = VD11PI*cos(-11*ANG_REF) - VQ11PI*sin(-11*ANG_REF);
						vMOD11_B = VD11PI*cos(-11*ANG_REF - 2.094395) - VQ11PI*sin(-11*ANG_REF - 2.094395);
						vMOD11_C = VD11PI*cos(-11*ANG_REF + 2.094395) - VQ11PI*sin(-11*ANG_REF + 2.094395);
					}

		 			// malha de controle para 13� harm�nica;
					if (AUX1 > (100/60))
					{
						ERRO_ID13_NMU = ERRO_ID13;
						ERRO_IQ13_NMU = ERRO_IQ13;
						ERRO_ID13 = 0 + ID13_MEDF;
						ERRO_IQ13 = 0 + IQ13_MEDF;

						VD13PI_NMU = VD13PI;
						VQ13PI_NMU = VQ13PI;
						VD13PI = VD13PI_NMU + (TAQU/(2*Ti_PIH13) + Kp_PIH13)*ERRO_ID13 + (TAQU/(2*Ti_PIH13) - Kp_PIH13)*ERRO_ID13_NMU;
						VQ13PI = VQ13PI_NMU + (TAQU/(2*Ti_PIH13) + Kp_PIH13)*ERRO_IQ13 + (TAQU/(2*Ti_PIH13) - Kp_PIH13)*ERRO_IQ13_NMU;

						vMOD13_A = VD13PI*cos(13*ANG_REF) - VQ13PI*sin(13*ANG_REF);
						vMOD13_B = VD13PI*cos(13*ANG_REF - 2.094395) - VQ13PI*sin(13*ANG_REF - 2.094395);
						vMOD13_C = VD13PI*cos(13*ANG_REF + 2.094395) - VQ13PI*sin(13*ANG_REF + 2.094395);
					}
					// fim do c�lculo dos erros e aplica��es dos controladores;

					if (fabs(vMOD2_A) > 1 || fabs(vMOD2_B) > 1 || fabs(vMOD2_C) > 1 || fabs(vMOD5_A) > 1 || fabs(vMOD5_B) > 1 || fabs(vMOD5_C) > 1 || fabs(vMOD7_A) > 1 || fabs(vMOD7_B) > 1 || fabs(vMOD7_C) > 1 || fabs(vMOD11_A) > 1 || fabs(vMOD11_B) > 1 || fabs(vMOD11_C) > 1 || fabs(vMOD13_A) > 1 || fabs(vMOD13_B) > 1 || fabs(vMOD13_C) > 1)
					{
						FLAGAUX6 = 1;	// prote��o caso mitiga��o venha a falhar;
					}

				}

				else
				{

					vMOD2_A = 0;
					vMOD2_B = 0;
					vMOD2_C = 0;
					vMOD5_A = 0;
					vMOD5_B = 0;
					vMOD5_C = 0;
					vMOD7_A = 0;
					vMOD7_B = 0;
					vMOD7_C = 0;
					vMOD11_A = 0;
					vMOD11_B = 0;
					vMOD11_C = 0;
					vMOD13_A = 0;
					vMOD13_B = 0;
					vMOD13_C = 0;

				}

				// tens�es de refer�ncia para chaveamento via SPWM (moduladoras);
				vMOD_A = vMOD1_A + vMOD2_A + vMOD5_A + vMOD7_A + vMOD11_A + vMOD13_A;
				vMOD_B = vMOD1_B + vMOD2_B + vMOD5_B + vMOD7_B + vMOD11_B + vMOD13_B;
				vMOD_C = vMOD1_C + vMOD2_C + vMOD5_C + vMOD7_C + vMOD11_C + vMOD13_C;
				MODS_VA = vMOD_A + 1.5;
				MODS_VB = vMOD_B + 1.5;
				MODS_VC = vMOD_C + 1.5;
				MODI_VA = vMOD_A - 1.5;
				MODI_VB = vMOD_B - 1.5;
				MODI_VC = vMOD_C - 1.5;

			}

			else
			{

				vMOD1_A = 0;
				vMOD1_B = 0;
				vMOD1_C = 0;
				vMOD2_A = 0;
				vMOD2_B = 0;
				vMOD2_C = 0;
				vMOD5_A = 0;
				vMOD5_B = 0;
				vMOD5_C = 0;
				vMOD7_A = 0;
				vMOD7_B = 0;
				vMOD7_C = 0;
				vMOD11_A = 0;
				vMOD11_B = 0;
				vMOD11_C = 0;
				vMOD13_A = 0;
				vMOD13_B = 0;
				vMOD13_C = 0;
				vMOD_A = 0;
				vMOD_B = 0;
				vMOD_C = 0;
				MODS_VA = 0;
				MODS_VB = 0;
				MODS_VC = 0;
				MODI_VA = 0;
				MODI_VB = 0;
				MODI_VC = 0;

			}

		}

		else
		{
			vMOD1_A = 0;
			vMOD1_B = 0;
			vMOD1_C = 0;
			vMOD2_A = 0;
			vMOD2_B = 0;
			vMOD2_C = 0;
			vMOD5_A = 0;
			vMOD5_B = 0;
			vMOD5_C = 0;
			vMOD7_A = 0;
			vMOD7_B = 0;
			vMOD7_C = 0;
			vMOD11_A = 0;
			vMOD11_B = 0;
			vMOD11_C = 0;
			vMOD13_A = 0;
			vMOD13_B = 0;
			vMOD13_C = 0;
			vMOD_A = 0;
			vMOD_B = 0;
			vMOD_C = 0;
			MODS_VA = 0;
			MODS_VB = 0;
			MODS_VC = 0;
			MODI_VA = 0;
			MODI_VB = 0;
			MODI_VC = 0;
			FLAGAUX2 = 1;
		}

		AUX3 = AUX3 + TAQU;
		if(AUX3 > 1/FRERED)
		{
			AUX3 = TAQU;
		}

		// VARI�VEIS DE SA�DA //

		fGblCCS = MODS_VA;

		fGblCCS_1 = MODS_VB;

		fGblCCS_2 = MODS_VC;

		fGblCCS_3 = MODI_VA;

		fGblCCS_4 = MODI_VB;

		fGblCCS_5 = MODI_VC;

		fCCS_6 = FLAGAUX0;

		

		// FIM EXEC //
	}
	(fCCS_6 == 0) ? PS_ClearDigitOutBitB((Uint32)1 << (33 - 32)) : PS_SetDigitOutBitB((Uint32)1 << (33 - 32));
	PS_ExitTimer1Intr();
}

interrupt void Task_1()
{
	PS_EnableIntr();

	PS_SetPwm3ph1Uvw(fGblCCS, fGblCCS_1, fGblCCS_2);
	PS_SetPwm3ph2Uvw(fGblCCS_3, fGblCCS_4, fGblCCS_5);
	PS_ExitPwm1General();
}


void Initialize(void)
{
	PS_SysInit(30, 10);
	PS_StartStopPwmClock(0);
	PS_InitTimer(0, 0xffffffff);
	PS_InitPwm3ph(1, 1, 5000*1, 0, 2335);	// pwnNo, waveType, frequency, deadtime
	PS_SetPwm3phPeakOffset(1, 2, 0.5, 1.0/2);
	PS_SetPwm3ph1AdcIntr(ePwmNoAdc, 1, 0);
	PS_SetPwm3ph1Vector(ePwmNoAdc, Task_1);
	PS_SetPwm3ph1Uvw(0, 0, 0);
	PS_StartPwm3ph1();

	PS_InitPwm3ph(2, 1, 5000*1, 0, 2335);	// pwnNo, waveType, frequency, deadtime
	PS_SetPwm3phPeakOffset(2, 2, (-2.5), 1.0/2);
	PS_SetPwm3ph2AdcIntr(ePwmNoAdc, 1, 0);
	PS_SetPwm3ph2Uvw(0, 0, 0);
	PS_StartPwm3ph2();

	PS_ResetAdcConvSeq();
	PS_SetAdcConvSeq(eAdcCascade, 0, 1.0);
	PS_SetAdcConvSeq(eAdcCascade, 1, 1.0);
	PS_SetAdcConvSeq(eAdcCascade, 2, 1.0);
	PS_SetAdcConvSeq(eAdcCascade, 3, 1.0);
	PS_SetAdcConvSeq(eAdcCascade, 4, 1.0);
	PS_SetAdcConvSeq(eAdcCascade, 5, 1.0);
	PS_SetAdcConvSeq(eAdcCascade, 6, 1.0);
	PS_AdcInit(0, !0);

	PS_InitDigitIn(17, 100);
	PS_InitDigitIn(19, 100);
	PS_InitDigitIn(21, 100);

	PS_InitDigitOut(33);

	PS_InitTimer(1,50);
	PS_SetTimerIntrVector(1, Task);
	PS_StartStopPwmClock(1);
}

void main()
{
	Initialize();
	PS_EnableIntr();   // Enable Global interrupt INTM
	PS_EnableDbgm();
	for (;;) {
	}
}

