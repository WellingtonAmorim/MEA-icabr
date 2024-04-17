//#define EIGENLIB			// uncomment to use Eigen linear algebra library

#include "fun_head_fast.h"

// do not add Equations in this area

MODELBEGIN

EQUATION("A")
// Produtividade
/* 
A produtividade da área plantada "A" depende do parâmetro "a" que representa a velocidade do aprendizado (ou velocidade de assimilação de conhecimento tecnológico. Quanto menor for este parâmetro, mais rápido será o aprendizado e maior a produtividade. 
*/ 

v[0] = V("a")*VL("A", 1) + (1 - V("a"))*V("Af") + norm(-100,100);

if (v[0] > 15000) {
    v[0] = 15000;
}

if (v[0] < 1000) {
    v[0] = 1000;
}

RESULT(v[0]) //Pendência calibrar
/*=================================ADDITION===========================*/
EQUATION("AM")

v[1]= AVE("A")
/*=================================ADDITION===========================*/

RESULT(v[1])

EQUATION("Af")
// Produtividade Final
/* 
"Af" define o aumento ou decréscimo da produtividade. Determina o limite da produtividade no final de cada simulação. O parâmetro "x" multiplica o grau de conhecimento tecnológico "T" e soma à produtividade final no tempo zero "A0" para cada agricultor.
*/

RESULT(V("x")*V("T") + V("A0"))

EQUATION("T")
// Grau de conhecimento tecnológico "T" 

/* É função do estoque de conhecimento comparado à média do mercado. A taxa de crescimento do estoque de conhecimento "z" é dada pela variação do conhecimento no período presente e o anterior (derivada do estoque de conhecimento no tempo) */

RESULT((V("z") - V("az"))/V("az"))


EQUATION("az")
// Average z
STAT("z");
RESULT(v[1])

EQUATION("z")
// Knowledge increase rate
RESULT((V("Z") - VL("Z", 1))/VL("Z", 1))

EQUATION("Z")
// Estoque de conhecimento
/* 
O estoque de conhecimento "Z" é função do investimento do agricultor "I", da sua capacidade de absorção "g", do efeito spillover "t", do total de investimento realizado por outros produtores "SUM("I") e do conhecimento externo "E" 
*/
RESULT(V("I") + V("g")*(V("t")*(SUM("I") - V("I")) + V("E")))


/////////////////////////BLOCO DE SELEÇÃO DA PROB///////////////////////////////////////////

EQUATION("mksim")
// Investment Market Share
RESULT(VL("Im", 1)/SUML("I", 1))

EQUATION("mksin")
// Investment Market Share
RESULT(VL("In", 1)/SUML("I", 1))


//inovar e imitar tem a mesma dificuldade, modular ou criar um outro parametro diferente de s
EQUATION("PROB_IM")
// Sucess Probability of Imitation
v[0] = 1;
v[1] = exp(-V("s")*V("Im"));
v[2] = v[0] - v[1];
v[3] = bernoulli(v[2]);
RESULT(v[3])

EQUATION("PROB_IN")
// Sucess Probability of Innovation
v[0] = 1;
v[1] = exp(-V("s")*V("In"));
v[2] = v[0] - v[1];
v[3] = bernoulli(v[2]);
RESULT(v[3])


EQUATION("TESTE")
RESULT(RND <= V("PROB_IM"))

///////////////////////////////////////////////////////////////////////////////////////////


EQUATION("TESTE")
RESULT(exp(V("In")))



EQUATION("g")
// Capacidade de Absorção futura (Inn = 0 IMITATOR, Inn = 1 INNOVATOR)
/*
É função da capacidade de absorção passada ("g", 1), do investimento realizado "I", de um parâmetro "b" que especifica a complexidade do conhecimento assimilado e da depreciação da capacidade de absorção "Dp"

*/

v[0]=0;

if (V("PROB_IM")==1)  
{        
if (V("PROB_IN")==0)
{
// Sucesso do Imitador 
   v[0] = max(VL("g", 1),VL("gmean", 1)); 
}

if (V("PROB_IN")==1)
{
  v[0] = VL("g", 1) + V("b")/sqrt(VL("In", 1)) +  max(VL("g", 1),VL("gmean", 1));
}
}

if (V("PROB_IM")==0)
{
if (V("PROB_IN")==0)
{
 v[0] = (min(VL("g", 1), VL("g", 1) + V("b")/sqrt(VL("Im", 1)) - V("Dp")) +  VL("g", 1) + (V("b")/sqrt(VL("In", 1))) - V("Dp"))/2;
 
}

if (V("PROB_IN")==1)
{
// Sucesso do Inovador
   v[0] = VL("g", 1) + V("b")/sqrt(VL("In", 1));
}
}



RESULT ( v[0] )



EQUATION("gmean")
// Farms AC average
v[0] = 0;
v[1] = 0;

CYCLE(cur, "FARM")
{
v[0] = v[0] + VLS(cur, "g", 1);
v[1] = v[1] + 1;
}
RESULT(v[0]/v[1])

EQUATION("C")
// Cost function
RESULT(((V("CO")*(1-(V("f")*V("g"))) + V("I"))))

EQUATION("p")
// Profit function
RESULT(V("REC") - V("C")) // alterado por wellington questão do C grande 


EQUATION("REC") // ALTERADO PARA TER AS CONDIÇÕES INICIAIS DESCRITAS NA TESE DO AUTOR
v[0]=V("price");
v[1]=V("A");
RESULT(v[0]*v[1])

EQUATION("RECINV")
RESULT(V("alpha")*VL("REC",1))

EQUATION("RECIMIT")
RESULT(V("beta")*VL("REC",1))

///////////////bloco investimento//////////////////////

EQUATION("In")
// Investment function inovation
RESULT(V("rin")*V("RECINV"))


EQUATION("Im")
// Investment function imitation
RESULT(V("rim")*V("RECIMIT"))


EQUATION("I")
RESULT(V("In")+V("Im"))



EQUATION("Am")
// Farms AC average
v[0] = 0;
v[1] = 0;

CYCLE(cur, "FARM")
{
v[0] = v[0] + VLS(cur, "A", 1);
v[1] = v[1] + 1;
}
RESULT(v[0]/v[1])



EQUATION("TESTE1")
RESULT(V("AmFarm")>= V("Am"))


EQUATION("AmFarm")
v[1]=VL("A",1);
v[2]=VL("A",2);
v[3]=VL("A",3);
v[4]=VL("A",4);
v[5]=VL("A",5);
v[6]=(v[1]+v[2]+v[3]+v[4]+v[5])/5;
RESULT(v[6])




EQUATION("rim")

if (V("AmFarm")>= V("Am")) 
{ 
v[7]=VL("rim",1); 

} else { // capacidade absortiva em t-1 menos a média / média (se a capacidade absortiva for inferior a média isso leva a um comportamento mais imitativo em relação ao inovativo. por outro lado, se for positivo ele tende a proporcionalmente direcionar os recursos para a atividade de inovação// operacionalização: criar um somatório no rim de forma a calcular a 

v[8]=1;
v[9]=100;
v[10]=norm(0,0.015); // choque positivo ou negativo a depender da distancia da absortive capacity individual em relação a média.
v[7]=VL("rim",1)-v[10]; 
}

RESULT(v[7])



EQUATION("rin")
v[0]=VL("rim",1); ///// 
RESULT(1-v[0])




MODELEND

// do not add Equations in this area

void close_sim( void )
{
	// close simulation special commands go here
	}

