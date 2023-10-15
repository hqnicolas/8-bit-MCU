# Projeto de Competência Semestral
- Componente Curricular de Eletrônica Aplicada e Microprocessadores
- Nicolas Pereira
- 27/09/2010
# Proposta: 
``
Este projeto foi desenvolvido com a utilização do PIC 16F877A, fonte de tensão continua, cristal de quartzo, teclado seis teclas, display LCD, motor e sensores.
Com este material desenvolvi um portão cujo acionamento é dado pela inserção de senha anteriormente cadastrada no sistema e informações exibidas em tempo real no display do dispositivo.
A senha pode ser cadastrada no sistema através do teclado por seqüência de teclas fixas a programação do dispositivo e requisição dos caracteres que antes foram cadastrados.
Informações como o estado do sistema, movimento do portão, acesso aberto ou travado com senha são exibidos no display.
Sensores fim de curso, instalados na polia do componente mecânico ditam a parada do motor em conjunto com um sensor de barreira que define a parada e a inversão do mesmo.
O teclado de seis teclas com quatro funções direcionais de noventa graus, tecla entrar e tecla sair será montado em matriz utilizando quatro pinos de um port.
existe a possibilidade de instalar uma placa de radio codificado externo para este acionamento.
``
# Software em C
- Este software permite o controle de acesso a um portão por meio de uma senha de quatro dígitos. A seguir, estão as principais funcionalidades e operações suportadas:

- Controle de Senha:
A senha de quatro dígitos pode ser editada usando a operação "incremento".
Ao pressionar "abrir", o software verifica a senha de combinação de quatro dígitos.
Uma vez logado, o portão pode ser aberto pressionando "abrir".
O portão pode ser fechado pressionando "fechar".
Após 5 segundos de inatividade, o display será desligado automaticamente.
Edição de Senha:

Após a edição de um dos caracteres da senha, se demorar 5 segundos, o caractere será apagado.
Quando o display é desligado e o portão está aberto, o usuário permanece logado.
Quando o display é desligado e o portão está fechado, o usuário é deslogado.
Edição da Senha:

- Para editar a senha, pressione os botões "sair" e "baixo" simultaneamente por 5 segundos.
A edição de senha requer login e a inserção da senha antiga.

Menu Principal:
- Login
- Abrir
- Fechar
- Cadastrar
- Login
- Inserir
