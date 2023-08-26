# ZINF-UFRGS
**ZINF - Um Jogo Inspirado em Zelda**

# Descrição:
ZINF é um jogo inspirado em clássicos como Zelda, desenvolvido como trabalho final para a disciplina de Algoritmos e Programação, do curso de Ciência da Computação, na UFRGS.  
Neste jogo, o jogador assume o papel de um herói em uma jornada épica, enfrentando monstros e conseguindo pontos.

![](https://github.com/dev-joseh/ZINF-UFRGS/blob/main/Imagens/ZINF.gif)

# Controles nos menus:
**W, S:** Mudam a opção em destaque.  
**ENTER:** Seleciona a opção em destaque.  
**Z:** Zera as pontuações no menu de pontuações.  

![](https://github.com/dev-joseh/ZINF-UFRGS/blob/main/Imagens/Menu.PNG)

# Controles durante a gameplay:
**W, A, S, D:** Movem o personagem pelo mundo do jogo.  
**K:** Ataca inimigos 3 blocos a frente do jogador.  
**P:** Pausa o jogo.  

# Dificuldades:
O jogo oferece cinco níveis de dificuldade:

Fácil:        Velocidade dos monstros reduzida, velocidade do jogador levemente aumentada, pontos recebidos em quantidade pequena.  
Normal:       Velocidade dos monstros e do jogador na configuração padrão, pontos recebidos em quantidade padrão.  
Difícil: 	    Velocidade dos monstros aumentada, velocidade do jogador levemente reduzida, pontos recebidos em quantidade um pouco maior.  
Impossível: 	Velocidade dos monstros muito aumentada, velocidade do jogador drasticamente reduzida, pontos recebidos em quantidade alta.  
NÃO TESTADA: 	Velocidade dos monstros causa epilepsia, jogador absurdamente lento, muitos pontos!  

![](https://github.com/dev-joseh/ZINF-UFRGS/blob/main/Imagens/Dificuldades.PNG)

_Escolha sabiamente sua dificuldade ou sofra as consequências!_

![](https://github.com/dev-joseh/ZINF-UFRGS/blob/main/Imagens/Game Over.PNG)

# Desenvolvimento:
Este jogo foi desenvolvido em linguagem C, utilizando a biblioteca Raylib.  
Alguns dos sprites utilizados no jogo foram fornecidos pelo nosso professor.  
Além disso, alguns efeitos sonoros foram retirados da internet para tornar a experiência mais imersiva.  

# Como Jogar:
Clone este repositório em sua máquina local.  
Certifique-se de que você possui a biblioteca Raylib (www.raylib.com) instalada.  
Compile o jogo usando o compilador C de sua escolha.  
Execute o jogo.  
Escolha sua dificuldade e comece sua aventura.  

# Criar níveis:
Para criar novos níveis, adicione um arquivo .txt com o nome "nivel@.txt" dentro da pasta niveis.  
O arquivo deve conter uma tabela de caracteres que representam cada objeto do jogo (B, P, J ou M)  
**B** - Background, é o chão que o jogador e os monstros pisam  
**P** - Pedra, obstáculo que impede a movimentação de monstros e do jogador  
**J** - Jogador, a posição inicial do jogador       **(MAX 1)**  
**M** - Monstro, posição inicial de um dos monstros **(MAX 25)**  
  
O jogo pode ter até 9 níveis, em sequência, iniciando do 1. Quando mais nenhum nível for encontrado durante a
gameplay, o jogo encerra e o jogador é levado para a tela de vitória.

# Contribuição:
Contribuições são bem-vindas! Sinta-se à vontade para corrigir bugs ou enviar solicitações de pull para melhorar o jogo.  
  
**Divirta-se jogando ZINF e boa sorte em sua aventura épica!**
