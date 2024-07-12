# Reescrevendo totalmente o suporte da GameSpy de 2000 a 2004 usando engenharia reversa em jogos da EA Games e Bungie

Author: João Vitor (@Keowu) - Security Researcher

## Introdução

Este artigo visa documentar e também apresentar meus passos durante os estágios de engenharia reversa de três grandes clássicos da minha infância e adolescência: Battlefield 1942, Vietnam e Halo CE. A ideia por trás desse artigo surgiu após eu encontrar meu antigo computador no sótão da minha casa e, ao ligá-lo, visualizar os ícones em meu desktop desses grandes clássicos. Ao final deste artigo, você será capaz de entender como funcionam as implementações e protocolos de comunicação desses jogos. Vamos reescrever totalmente todas as listas de servidores desses jogos, permitindo que voltem à vida como eram em sua época de glória, explicando cada detalhe e fornecendo todo o código-fonte para posteriores pesquisas e usos, ou até mesmo resgatar o sentimento de nostalgia dos clássicos. Além disso, você também será capaz de reverter e reviver todos os clássicos que têm a GameSpy SDK embutida neles.

![#0](imagens/intro.png)

## Table of Contents
1. [Introdução](#introdução)
2. [Uma leve motivação](#uma-leve-motivação)
3. [GameSpy da glória a Decadência](#gamespy-da-glória-a-decadência)
4. [Battlefield 1942 - GameSpy 2002](#battlefield-1942---gamespy-2002)
    1. [Analisando binário](#analisando-binário)
    2. [Revertendo pacotes](#revertendo-pacotes)
    3. [Revertendo o código fonte e implementação da Gamespy](#revertendo-o-código-fonte-e-implementação-da-gamespy)
    4. [[Bonus] Explorando segurança das chaves de registro para abusar do problema de serial dos sevidores para desconectar jogadores](#bonus-explorando-segurança-das-chaves-de-registro-para-abusar-do-problema-de-serial-dos-servidores-para-desconectar-jogadores)
    5. [Como era gerado a hash para a serial key ? a mesma usada nos servidores para verificação ?](#como-era-gerada-a-hash-para-a-serial-key-a-mesma-usada-nos-servidores-para-verificação)
    6. [Analisando a implementação de verificação de servidores da Gamespy SDK no Battlefield 1942](#analisando-a-implementação-de-verificação-de-servidores-da-gamespy-sdk-no-battlefield-1942)
    7. [Escrevendo um parser para pacotes](#escrevendo-um-parser-para-pacotes)
    8. [Analisando e criando ideias para modificações](#analisando-e-criando-ideias-para-modificações)
    9. [Analisando padrões de código](#analisando-padrões-de-código)
    10. [Escrevendo uma nova Master Server List provider](#escrevendo-uma-nova-master-server-list-provider)
    11. [Testando o projeto](#testando-o-projeto)
5. [A Gamespy de 2004 (Usada pela Bungie e EA)](#a-gamespy-de-2004-usada-pela-bungie-e-ea)
    1. [Um pouco do posicionamento da EA Games x Bungie](#um-pouco-do-posicionamento-da-ea-games-x-bungie)
    2. [Como ocorre a comunicação de rede](#como-ocorre-a-comunicação-de-rede)
    3. [Analisando os Pacotes](#analisando-os-pacotes)
    4. [Escrevendo um parser de pacotes para Gamespy 2004](#escrevendo-um-parser-de-pacotes-para-gamespy-2004)
    5. [Analisando e criando ideias para modificações GameSpy 2004](#analisando-e-criando-ideias-para-modificações-gamespy-2004)
    6. [Analisando padrões de código da GameSpy 2004](#analisando-padrões-de-código-da-gamespy-2004)
    7. [Reescrevendo a MasterServer Provider para a GameSpy 2004](#reescrevendo-a-masterserver-provider-para-a-gamespy-2004)
    8. [Testando o projeto](#testando-o-projeto-1)
6. [Ideias Extras](#ideias-extras)
    1. [Gameloader](#gameloader)
    2. [Dicas de modo Janela para Desenvolvimento](#dicas-de-modo-janela-para-desenvolvimento)
    3. [TeaDelKew](#teadelkew)
    4. [Error track com MiniDumps](#error-track-com-minidumps)
7. [Bla, Bla Juridico](#bla-bla-juridico)
8. [Revisores, Testers e Agradecimentos especiais](#revisores-testers-e-agradecimentos-especiais)
9. [Conclusão](#conclusão)
10. [References](#references)

### Uma leve motivação

No clima nostálgico que esta pesquisa nos apresenta, outro grande clássico do mundo dos games (em especial da minha terra natal, o Brasil), do meu youtuber favorito dos games, Zangado, e suas maravilhosas palavras de reflexões dos games e da vida.

[![SER GAMER... Zangado](https://img.youtube.com/vi/j-wH8EPJ03U/0.jpg)](https://www.youtube.com/watch?v=j-wH8EPJ03U)

PS: Não espero que um não nativo brasileiro entenda o peso desta referência (apenas respeite o momento), sendo assim, siga com o decorrer do artigo.

Essa pesquisa tem uma certa importância pessoal. Enquanto escrevia, lembrei-me de ótimos momentos com meus amigos do Ensino Fundamental e Médio, de quanto me diverti jogando esses clássicos, da quantidade de pessoas que conheci (brasileiros e estrangeiros) e de como a simplicidade nos faz tão felizes. Nessa época, com certeza, eu não tinha acesso aos melhores hardwares do mercado, mas o pouco que tinha me moldou a lutar e alcançar meus objetivos. Não tinha um centavo, mas eu sempre tive uma visão. Espero sinceramente que você não leia isso com o olhar de um idiota e superioridade, mas que afogue a sua própria idiotização e transforme suas ideias em uma incansável luta pela própria evolução!

Além disso, como de costume em meus artigos, recomendo uma música, e desta vez escolhi: [Capital Inicial - Primeiros Erros](https://www.youtube.com/watch?v=jabmx3QoJGA), edit: ouvir depois de ler.

### GameSpy da glória a Decadência

Se você jogou algum jogo clássico de franquias como Battlefield, Halo, Arma, Crysis, Star Wars, até jogos multiplayer do PlayStation 2 (sim, isso era possível) e Nintendo Wii, com toda certeza você usou algum serviço da GameSpy.

![#1](imagens/gslogo.png)

GameSpy foi criada pelo engenheiro de software Mark Surfas e provia apenas serviços de listagem de browser para o Quake (1996). Também foi nessa época que começaram a comercializar o serviço para empresas desenvolvedoras de jogos (como a EA e seu Battlefield 1942, em 2002). Porém, foi em 2004 que o número de títulos que utilizavam o serviço teve um crescimento exponencial com a aquisição pela IGN.

A GameSpy mostrava-se viável para desenvolvedores, em uma época que manter um serviço de multijogadores estável e funcional demandaria muito tempo de desenvolvimento. A SDK provia aos desenvolvedores tudo que era preciso, desde server browser até autenticação (como no caso de alguns jogos como Battlefield 2, Battlefield 2142 e jogos do PlayStation 2). A SDK era multiplataforma e facilmente portável para diversas plataformas, fornecendo apenas uma callback para os desenvolvedores obterem os dados necessários, focando apenas na criação e design do server browser dos jogos. Até mesmo as interfaces e aplicações server-side auto-hospedadas pelos jogadores utilizavam essa mesma base de SDK provida pela GameSpy.

![#2](imagens/gsad.jpg)

Infelizmente, manter um serviço desse tamanho era extremamente caro e pouco rentável. Foi então que, em abril de 2014, a IGN decidiu encerrar os seus serviços, matando completamente diversos títulos clássicos de várias franquias. Na época, a IGN ofereceu o código-fonte completo e o backend às empresas que usavam seus serviços para que pudessem hospedar por conta própria e lançar patches corrigindo os jogos. Algumas empresas, como a EA Games, apenas ignoraram esse fator e deixaram suas franquias clássicas morrerem. Outras, como a Bungie, hospedaram o backend recebido da IGN em algum lugar. Por exemplo, a Bungie mantém uma instância EC2 da Amazon rodando o serviço de server browsing para seus clássicos, como o Halo: Combat Evolved, além de, é claro, lançarem um patch aos seus jogos corrigindo este problema. Porém, isso são exceções.

## Battlefield 1942 - GameSpy 2002

Battlefield 1942 foi o primeiro clássico da franquia Battlefield, lançado pela EA Games em 2002. Ele contava com uma implementação de uma das primeiras SDKs desenvolvidas pela GameSpy. Esta versão já continha a criptografia proprietária de pacotes apelidada de ```GOA``` (GameSpy Online Access). Este algoritmo combinava chaves randômicas simétricas e compressão de dados, além de rounds shuffle com XOR em volta do payload recebido do MasterServer. Esses dados eram então parseados, verificados, obtinham-se mais informações sobre os servidores e, então, retornados em uma função callback fornecida pela própria EA Games ao inicializar a struct de configuração do GameSpySdk (rotina essa apelidada de ```SBListCallBackFn```, com base em informações obtidas em metadados de binários dessa época).

#### Analisando binário

O binário original do jogo Battlefield 1942 (versão de CD) não continha nenhum DRM como os atuais em seu binário. É claro que possuía alguma proteção antipirataria, como é o caso do ```SafeDisc```, que utilizava o bom e velho driver vulnerável ```secdrv.sys```, que apenas impedia sua execução sem o disco original. Facilmente ignorado com o advento da pirataria no Windows XP, atualmente é bem difícil encontrar essas versões, já que apenas edições de colecionadores estão disponíveis no mercado. Além disso, no Windows 10 é inútil tentar carregar esse driver, já que em 2015 a Microsoft simplesmente bloqueou seu carregamento devido à compatibilidade e segurança, já que existiam exploits para ele. Dado isso, os desenvolvedores foram avisados bem antes sobre este problema, já que em 2012 a própria EA Games removeu a SDK do binário do jogo, adicionando então o Origin-DRM. Origin DRM (agora EA-Launch DRM) utiliza AES para criptografar os dados de seções do binário PE de seus jogos, e as chaves são armazenadas conforme a licença do usuário em um arquivo apelidado de ```BF1942.par```. O processo de loading através desse DRM descriptografa todos os dados através de uma nova seção adicionada no binário PE sem nome:

![#3](imagens/encsection.png)

Essa nova stub, feita manualmente em assembly, carrega uma DLL apelidada de ```Activation.dll``` e chama seu export também sem nome, utilizando apenas o ```ordinal```. A partir disso, a DLL carregada reconstrói toda Tabela de Importação e descriptografa o conteúdo da seção ```.text``` em blocos de 16 bytes e, por fim, chama o Entrypoint original. Veja a nova stub adicionada nos jogos responsável por fazer esse procedimento:

![#4](imagens/ealoader.png)

O código do jogo é o mesmo das versões pirateadas encontradas na internet ```v1.61```, com a pequena diferença de não possuir nenhuma alteração além de remover o DRM SafeDisc.

Uma curiosidade é que os desenvolvedores da DICE usaram o ```VirtualXP``` e uma nova branch do SVN para gerar as builds do jogo com a implementação do novo DRM:

![#5](imagens/compilemetadata.png)

Não entrarei em maiores detalhes de como esse DRM funciona, até porque existem conteúdos explorando outras partes do mesmo. Além, é claro, de não querer problemas judiciais (dado que isso é uma pesquisa para reviver os jogos e não pirateá-los). Porém, deixo a curiosidade da criatividade do time de segurança da EA Games ao proteger seus jogos.

![#6](imagens/ea_money_shit.gif)

Para continuar com a versão da Origin (EA Launcher), você pode simplesmente deixar o loader fazer o trabalho pesado: reconstruir a IAT e apenas definir o EntryPoint (coisa que não será demonstrada aqui). Porém, apenas saiba que tudo que for demonstrado aqui é aplicável na versão da Origin/EA Launcher, dado que a base de código é exatamente a mesma, só que sem o DRM antigo.

Hoje em dia, se você apenas obter uma cópia do Battlefield 1942 de algum lugar, como Origin/EA Launcher, disco ou de algum lugar nos confins da internet, e for jogar online, simplesmente não vai ser possível, já que a lista de servidores não existe mais. Porém, se você encontrar algum endereço IP de servidor na internet, ou você mesmo hospedar um servidor, é possível jogar adicionando-o aos favoritos.

**Você pode se perguntar por que isso ocorre?**

Basicamente, quando você já conhece um endereço IP, não há necessidade da masterserver list, já que a própria SDK da Gamespy envia requisições solicitando detalhes do servidor presente na lista e, como ele está ativo, apenas retorna as devidas informações para você.

**Agora você deve estar se perguntando como isso ocorre?**

Vamos descobrir a partir de agora, analisando e revertendo completamente o Battlefield 1942 com a implementação da SDK da Gamespy 2002 presente nele.

![#7](imagens/genericanimestuff2.gif)

#### Revertendo pacotes

Algumas pessoas tiveram acesso à SDK completa do Gamespy no momento de fechamento dos servidores da Gamespy, principalmente grandes servidores desses jogos que entraram em contato para obtê-la, não sendo o meu caso. Durante a demonstração de como funciona a comunicação entre cliente e masterserver, e cliente e servidor, usarei minha própria reimplementação da lógica usada pelo Masterserver da Gamespy revertido e logo mostrarei a implementação no binário analisado do jogo.

O Battlefield 1942 utiliza as seguintes portas:

1. **28900** - Para comunicação com o Masterserver list provider através do protocolo TCP.
2. **23000** - Para comunicação e obtenção de informações dos servidores através do protocolo UDP.
3. Demais conexões utilizam o protocolo TCP e não serão abordadas.

Quando uma nova solicitação de atualização da server browser ocorre, através do clique do botão abaixo:

![#8](imagens/bf1942_1.png)

Uma conexão TCP com o endereço do Masterserver através da porta 28900 é efetuada, e quando a conexão é aceita, a primeira requisição é recebida pelo cliente:

![#9](imagens/bf1942_2.png)

O comando recebido pelo cliente é o seguinte:

```\\basic\\secure\\MASTER```

Este comando indica que um backend da Gamespy para server browser provider está disponível para ser consultado. Trata-se de uma apresentação do Masterserver ao cliente conectado.

Em seguida, o Masterserver espera que o cliente forneça uma query para o jogo em questão. Essa query é composta pelas seguintes informações:

1. Gamename.
2. Versão do Jogo.
3. Location, sempre por padrão zero(isso é diferente no caso de implementações do Playstation 2, que possuiam jogos diferentes por região).
4. Validação, Uma chave hardcoded padrão. única para cada jogo, podendo ser considerado uma chave de API.
5. Enctype, padrão 2, porem isso se referia a versão do algoritmo do GOA(Gamespy Online Access)
6. O magic delimitador para indicar o final dos frames, nesse caso sempre ASCII "final", isso vai fazer mais sentido ao decorrer da explicação.
7. Query ID com o padrão sempre sendo 1.1

![#10](imagens/bf1942_3.png)

Seguido disso, temos a última requisição a qual, com base na query acima, solicita que o Gameserver Provider envie o buffer comprimido e protegido com o algoritmo GOA (Gamespy Online Access):

![#11](imagens/bf1942_4.png)

Diferente do ```query```, o list não tem nenhum significado ou configuração a ser explicada; ele apenas fará o request do buffer contendo o payload com os servidores a serem usados.

Por último, após todo esse processo, o buffer protegido é enviado pelo masterserver provider totalmente criptografado (não sendo o meu caso, porque eu removi o algoritmo já que minha solução simplesmente não precisa dele):

![#12](imagens/bf1942_5.png)

Antes de explicar como isso foi implementado e mostrar toda a explicação da engenharia reversa por trás, esta é uma masterserver provider real, hospedada no Brasil, escrita e revertida no meu tempo livre, que agora vai se tornar Open Source.

![#13](imagens/genericanimestuff1.gif)

Vamos olhar como as requisições ocorrem diretamente na implementação da SDK da Gamespy presente no Battlefield 1942.

#### Revertendo o código fonte e implementação da Gamespy

Vamos iniciar pelo procedimento apelidado por mim de ```decrypt_tcp_traffic```. Uma curiosidade aqui é que a SDK da Gamespy era completamente desenvolvida em C, e deveria ser mesmo, até porque isso facilitava muito ao portar para outras plataformas/arquiteturas. Bom, continuando, voltando à explicação do procedimento, ele é responsável por definir configurações ao socket, ler o buffer que estiver pendente nele, descomprimir e descriptografar, tudo isso com o tamanho máximo de 2047 (sendo esse o tamanho máximo que um buffer da Gamespy pode suportar). Além, é claro, de parsear os dados e adicionar em uma callback (da própria Gamespy) para que sejam validados e, por fim, adicionados a uma lista ligada e passados à callback fornecida por quem implementava a SDK, nesse caso, a DICE. Outras callbacks também eram implementadas, como para tratativas de erros, tudo fará mais sentido conforme eu apresento.

Começando do início, temos uma chamada para ```select``` e, em seguida, uma chamada para ```recv```. O objetivo aqui é monitorar eventos para leitura do socket, definindo um timeout de 0 segundos para cada evento de leitura do socket TCP estabelecido. Retornando um código de erro ao GameSpy Switch (um bloco de código que interage em loop, responsável por responder a eventos externos direcionados ao SDK, além de eventos do socket como de apresentação e autenticação no Masterserver provider), o código retornado em questão é ```STATUS_TIMEOUT_ERROR``` (obviamente, isso foi definido por mim com base na análise do comportamento do código e minha interpretação dele). Logo após, ocorre de fato a leitura do buffer com tamanho máximo de ```2047```. Esse valor constante é o tamanho máximo que um payload (conjunto criptografado ou descriptografado com os frames de servidor para consulta) pode assumir. Aqui, perceba também que uma global é utilizada, ```readed_buffer```, uma DWORD que, em questão, armazena o valor já lido com o tamanho do payload anteriormente, caso uma nova interação seja necessária. Isso ocorre, por exemplo, se, por algum motivo, o payload estiver corrompido.

![#14](imagens/bf1942_6.png)

Todos os dados lidos do payload são armazenados em uma global, em um ponteiro composto por duas globais. E aqui mais uma vez chama a criatividade da implementação. A primeira global, ```FirstFrameByte```, é um byte, seguido de ```readed_buffer```, que carrega o offset/tamanho do buffer anterior. Cabe destacar que, na maioria das vezes, esse valor se assume ```0``` por padrão.

Você deve estar se perguntado agora:

**Como assim um único byte não vai transbordar?**

![#15](imagens/genericanimestuff3.gif)

A resposta é sim! E esse é o objetivo mesmo. O desenvolvedor alinhou muito bem os dados. Ao ler o buffer, sempre será com um tamanho superior a um byte, desta forma sobrescrevendo outra variável global apelidada de ```frame_buffer```. Ela, de fato, vai carregar o buffer sem o byte mágico (que, por coincidência, é sempre um único byte que, com um XOR de ```0xEC```, resulta em algo superior a 0, sendo que zero indica que a geração do payload foi negada). Veja um exemplo:

![#16](imagens/bf1942_7.png)

Agora com os dados corretos, hehe:

![#17](imagens/bf1942_8.png)

Seguido disso, temos as verificações do tamanho do buffer lido:

![#18](imagens/bf1942_9.png)

Separei este trecho para mostrar coisas curiosas para você, leitor. Aqui podemos ver as validações feitas pela Gamespy em relação ao tratamento de erros quando um buffer não é obtido do socket TCP. Mas perceba que, como mencionado por mim anteriormente, uma callback registrada por quem implementou a SDK em determinado jogo é registrada. ```clsSocket->GamespyCallback``` nada mais é do que o endereço de um procedimento da própria DICE que receberá o código de erro fornecido pela Gamespy e com base em um switch case tratará exibindo a devida mensagem de erro para ele. Uma segunda curiosidade aqui é que os parâmetros na declaração podem assumir qualquer coisa por quem implementar, já que é um ```PVOID```. Nesta mesma callback, as informações da lista ligada de servidores são retornadas.

Bom, indo mais a fundo, vamos entender como os dados de um raw buffer, que é um payload, vão ser tratados até obtermos informações relevantes que nos permitam entender o que trafega nesse procedimento.

![#19](imagens/bf1942_10.png)

A partir de agora, vamos ver a utilização do GOA (Gamespy Online Access) em dois procedimentos, o primeiro ```decompress_one``` e o segundo ```decompress_two```, além do primeiro estágio de parsing dos dados em um ponteiro para uma struct de dados revertida por mim, a ```server_data_struct```.

A primeira verificação em ```clsSocket->already_decrypted != -1``` é usada nos casos já mencionados acima, onde a descriptografia dos payloads já foi feita anteriormente, porém estava corrompida de alguma forma. Então não há necessidade de trabalhar com os cabeçalhos novamente, apenas com os dados do body do payload. O algoritmo responsável por isso está no procedimento ```decompress_two```. Avançando, caso seja a primeira interação com os dados do payload, veremos todo o processo de descompressão e inicialização da chave e decodificação do cabeçalho de dados. O marco para início dessa etapa é a comparação em ```already_decrypted == -1```:

![#20](imagens/bf1942_11.png)

Logo no início desta etapa, a primeira verificação usando o byte mágico 0xEC ocorre, seguido também da inicialização da chave que está no cabeçalho do payload através da operação de XOR ```frame_buffer[i] ^= clsSocket->ucRandomicKeyBytes[i + 40]```. A cada inicialização, as chaves em ```ucRandomicKeyBytes``` mudam e são construídas com base em um rand, mas esse é só o primeiro layer de criptografia em cima dos dados que ainda contam com a compressão. Após a inicialização da chave, o procedimento ```decompress_one``` é usado para descriptografar o cabeçalho do payload. Logo após isso, já temos o valor do offset para o body do payload, que é determinado pelo XOR do byte mágico com 0xEC (```FirstFrameByte ^= 0xECu```), onde era utilizado para acessar ```frame_buffer```, interpretado como uma referência ```PVOID``` que posteriormente identifiquei se tratar de uma struct (pois era acessada via índices no assembly, similar ao padrão adotado por structs). Dessa forma, criei a seguinte struct:

```c++
struct server_data_struct {

  DWORD server_ip;
  WORD server_port;

};
```

Onde cheguei ao seguinte resultado, facilitando melhor nossa visualização do que ocorria:

```c++
gameservers = (server_data_struct *)&frame_buffer[FirstFrameByte]
```

Onde, por fim, era utilizada como argumento para que o conteúdo do body do payload fosse descriptografado com uma chamada para "decompress_two", no seguinte trecho:

```c++
decompress_two(clsSocket->ucBuffer, gameservers, (PVOID)(&FirstFrameByte + readed_buffer - (UINTPTR)gameservers));
```

Essa era toda a rotina de criptografia e trabalho com o frame adotado pela Gamespy nessa versão. Apesar de ter feito um porte completo dessa rotina de descriptografia mais adiante em x86, eu não dediquei tempo em analisá-la a fundo. No entanto, ela era a mesma compartilhada por diversos jogos que usavam essa versão da SDK. Caso você se interesse em entender, recomendo ler o paper de outro pesquisador independente, ```LUIGI. Auriemma```, em específico em um paper apelidado de ```GS enctype2``` disponível nas referências do artigo. Mais adiante, introduzi uma nova criptografia nesses frames utilizando uma implementação modificada e pessoal do XTEA, apelidada de ```TeaDelKew```.

Logo após todo esse processo em relação ao algoritmo de descriptografia, chegamos à etapa de criação da lista ligada responsável por criar uma fila de processamento para verificar o estado do servidor utilizando o protocolo ```UDP``` com a porta padrão ```23000```.

![#21](imagens/bf1942_12.png)

Nesse procedimento, cada membro da struct é acessado: o IPv4, a porta, e o ponteiro da struct é incrementado em 6 bytes, o tamanho exato da própria struct para acessar o próximo conjunto de dados. Logo em seguida, todos esses dados são passados como argumento para a lista ligada em ```add_to_list_to_check_status_via_UDP```. 

Se você estiver se perguntando como é definido o tamanho máximo do payload nesse parsing, ele ocorre com base na assinatura (UTF-8) ```\\final\\```:

![#22](imagens/bf1942_13.png)

Nesse procedimento, quando o último frame for parseado, endFlag assumirá o valor padrão 7, e então o procedimento é encerrado, tudo isso enquanto não for encontrada a assinatura ```\\final\\```, que delimita exatamente o tamanho máximo caso algo falhe.

Vamos agora dirigir nossa atenção em como a validação dos servidores é feita pela Gamespy SDK usando o protocolo UDP. Essa verificação pode ser considerada uma espécie de heart-beat, já que a comunicação ocorre apenas para obter detalhes do servidor como números de jogadores, mapas e nome do mapa jogado. Vamos iniciar analisando os pacotes:

![#23](imagens/bf1942_14.png)

Quando a GameSpy SDK verifica um servidor, o primeiro comando enviado é o ```\status\```. Este comando é usado para solicitar as informações adequadas de heartbeat do servidor em questão. O servidor então responde com outros payloads, como exemplificado abaixo:

```
\gamename\bfield1942\gamever\v1.61\language\English\location\1033\averageFPS\0\content_check\0\dedicated\1\gameId\bf1942\gamemode\openplaying\gametype\ctf\hostname\NOME DO SERVIDOR | LOL NOME DO SERVIDOR\hostport\14567\mapId\BF1942\mapname\wake\maxplayers\64\numplayers\36\password\0\reservedslots\0\roundTime\1200\roundTimeRemain\274\status\3\sv_punkbuster\0\tickets1\0\tickets2\4\unpure_mods\\version\v1.61\active_mods\bf1942\allied_team_ratio\1\allow_nose_cam\yes\auto_balance_teams\on\axis_team_ratio\1\bandwidth_choke_limit\0\content_check\0\cpu\3766\external_view\on\free_camera\on\game_start_delay\15s\hit_indicator\on\kickback\10%\kickback_on_splash\10%\name_tag_distance\200\name_tag_distance_scope\350\number_of_rounds\1\soldier_friendly_fire\40%\soldier_friendly_fire_on_splash\40%\spawn_delay\4s\spawn_wave_time\7s\sv_punkbuster\0\ticket_ratio\250%\time_limit\30\tk_mode\forgive\unpure_mods\\vehicle_friendly_fire\40%\vehicle_friendly_fire_on_splash\0%\queryid\6903.1
```
![#24](imagens/genericanimestuff4.gif)

**Ficou confuso com as informações retornadas?**

Primeiramente, na época do lançamento, "JSON" não era tão comum assim. Na verdade, ele estava apenas começando a ser utilizado (já que surgiu no mesmo ano). Mas isso não é um problema, exceto pelas versões de 2005/2006 que ainda utilizavam XML nas comunicações da GameSpy, o que era padrão para o sistema de registro de jogadores. Já o modelo apresentado era comum e funcionava bem para o padrão necessário. Esse padrão não era exclusivo da DICE/EA; outros jogos da época compartilhavam o mesmo modelo. A diferença estava apenas nas informações recebidas, pois cada jogo as interpretava de maneira diferente. Alguns padrões eram mantidos, que destacarei agora:

- **gamename** - Representa o nome do jogo para o qual o servidor estava configurado, neste caso "bfield1942".
- **gamever** - Representa a versão do jogo, neste caso v1.61 (a versão mais atual do jogo). Se essa versão fosse diferente do cliente, um callback de erro era acionado.
- **hostname** - Representa o nome do servidor em questão.
- **hostport** - Representa a porta do servidor.
- **queryid** - Representa o ID da consulta feita naquele servidor, neste caso 6903.1.

Esses eram apenas os campos padrão do frame (que outros jogos também possuíam), mas a GameSpy era altamente adaptável e todos os outros campos eram específicos para Battlefield 1942.

A primeira parte apresentada era apenas a introdução recebida do servidor. Outras informações padrões também eram recebidas, como por exemplo, informações sobre os jogadores atuais no servidor, K/D (Kill, Death), ping e tempo jogado, além, é claro, da keyhash (algo interessante que será explicado em breve para que serve).

```
\deaths_0\2\keyhash_0\e09833b4065c325fbe042d4c7cf8bba9\kills_0\0\ping_0\41\playername_0\[Admin]Cher\score_0\0\team_0\1\deaths_1\10\keyhash_1\d8e803a4c432c5dbc129805bb13c5ae9\kills_1\0\ping_1\52\playername_1\Jim Lahey\score_1\13\team_1\1\deaths_2\0\keyhash_2\92ff73033c3351d61131b3129990aa77\kills_2\0\ping_2\17\playername_2\jake from state farm\score_2\0\team_2\1\deaths_3\7\keyhash_3\114d0844ea3df43d48840016f2b4a7ae\kills_3\17\ping_3\24\playername_3\TheCalmingClam\score_3\34\team_3\2\deaths_4\2\keyhash_4\93b6ad5f0452ee5ff76702ae3f071e9e\kills_4\19\ping_4\21\playername_4\mediocre\score_4\23\team_4\2\deaths_5\4\keyhash_5\860bd844411700821c06d91751a22f13\kills_5\10\ping_5\25\playername_5\*Incognito*\score_5\26\team_5\2\deaths_6\12\keyhash_6\e189e41fc7254f4f1ad351b0aadeeeeb\kills_6\4\ping_6\151\playername_6\GLIER\score_6\13\team_6\2\deaths_7\6\keyhash_7\8f2cbe5d55b6f974199a0136b91791c4\kills_7\7\ping_7\31\playername_7\Lagtastic\score_7\26\team_7\1\deaths_8\7\keyhash_8\d0db7b12341ec39e07dbf686d7c0663e\kills_8\1\ping_8\77\playername_8\papi\score_8\12\team_8\2\deaths_9\9\keyhash_9\57592dcf0786e10a376577ad33c9d8d6\kills_9\7\ping_9\59\playername_9\Meuse\score_9\13\team_9\1\deaths_10\7\keyhash_10\8e024e9945e8cef5565e7eb67c63356b\kills_10\7\ping_10\44\playername_10\Sorrowful Rice\score_10\40\team_10\2\deaths_11\17\queryid\6903.2
```

Como mencionado, todos os campos eram personalizados para Battlefield 1942, ao contrário de outros jogos. No entanto, ainda havia alguns padrões, como o próprio ```queryid```. A ```keyhash``` era exatamente o hash do seu serial de ativação armazenado em algum lugar do sistema operacional (registro de chave neste caso).

Uma curiosidade do jogo é que a verificação do serial era feita pelo lado do cliente, funcionando da seguinte maneira: imagine que você deseja entrar em um servidor, mas seu serial é o mesmo de um jogador já conectado. Isso era validado pelo campo ```keyhash```. Caso seu serial fosse o mesmo de um jogador já conectado, você receberia um erro (um callback de verificação da GameSpy, conforme detalhado anteriormente).

**Um problema aqui é que se você copiar uma dessas keyhashes e substituí-la pela sua, você pode se passar por outro jogador.**

![#24](imagens/genericanimestuff5.gif)

##### [Bonus] Explorando segurança das chaves de registro para abusar do problema de serial dos servidores para desconectar jogadores

Se você também estiver curioso sobre onde essas informações de serial estão armazenadas, nada melhor do que crackear um keygen/changer, obviamente sem entrar no âmbito da pirataria. Eu reverti um antigo changer de serial do Battlefield 1942; esse changer usava o packer ```ASPack``` e era completamente feito em ```Delphi```:

![#25](imagens/bf1942_15.png)

![#26](imagens/bf1942_16.png)

Este software se tratava apenas de um changer. Seu objetivo era apenas alterar as chaves de ativação do jogo. Ele não era um keygen. Sua única validação era verificar se o serial tinha o tamanho de 22 dígitos e apenas o gravava em duas chaves de registro sem nenhuma segurança, como demonstrado abaixo:

```\\SOFTWARE\\Electronic Arts\\EA GAMES\\Battlefield 1942\\ergc\\```

![#27](imagens/bf1942_17.png)

```\\SOFTWARE\\Aps\\cdkey\\```

![#28](imagens/bf1942_18.png)

###### Como era gerada a hash para a serial key? A mesma usada nos servidores para verificação?

Para responder a essa pergunta, podemos olhar a própria implementação do Battlefield 1942:

![#29](imagens/bf1942_19.png)

O valor armazenado na chave de registro é utilizado para gerar uma hash MD5 dos valores. É possível confirmar que se tratava de um MD5 apenas observando as constantes de inicialização presentes no procedimento "generate_data_md5":

![#30](imagens/bf1942_20.png)

Além disso, o output coincide com alguns softwares de keygen disponíveis na internet, além do meu próprio teste para confirmar:

![#31](imagens/bf1942_21.png)

Se você está se perguntando o que aconteceria se dois seriais com a mesma hash estivessem em um mesmo servidor, ambos levariam disconnect, e podemos abusar disso apenas hookando um procedimento de implementação do MD5. Será que podemos considerar uma vulnerabilidade? Fica a sua própria interpretação pessoal sobre o assunto.

![#32](imagens/genericanimestuff6.gif)

##### Analisando a implementação de verificação de servidores da Gamespy SDK no Battlefield 1942

Após entendermos como o serial funcionava e suas vulnerabilidades de segurança, agora vamos retornar nossa atenção para ver como a verificação de servidores era implementada pela Gamespy SDK, diretamente no fonte da implementação do Battlefield 1942.

O procedimento apelidado por mim de ```receive_data_from_masterserver``` é responsável por verificar cada servidor individualmente da lista anteriormente obtida com o parseamento das informações do payload recebido da masterserver. Com cada frame (entrada de struct na lista), verificando-se o tamanho total de servidores na mesma, inicia-se o envio do comando de query para cada servidor individualmente através de uma chamada para "sendto" usando o índice 0 da command query:

![#32](imagens/bf1942_22.png)

A global mencionada ```g_command_to_send``` contém todos os comandos que podem ser enviados durante as queries, claramente, com a devida ordem correta aos servidores. Porém, aqui apenas o índice ```5``` é utilizado, pois indica-se o início de uma nova query de status de um servidor no qual já conhecemos:

![#33](imagens/bf1942_23.png)

```! Apesar de não demonstrado aqui, os demais comandos também podem ser usados para você obter diversas informações dos servidores Gamespy, indo desde informações básicas, regras, players e regras.```

A flag ```status``` força que todas as informações possíveis do servidor sejam enviadas. Isso ocorrerá até que um ```\\final\\``` seja encontrado, indicando o final dessas informações, como podemos comprovar com base no entendimento do código original:

![#34](imagens/bf1942_24.png)

A cada informação obtida, uma callback da Gamespy registrada pela DICE/EA receberá os dados, os parseando conforme desejado, de maneira separada e já organizada. Essa callback é chamada até que todos os frames sejam recebidos, delimitados pela tag "final".

Como curiosidade e também se você estiver se perguntando, sim, existe uma callback de tratativas de erros, caso alguma coisa falhe durante o recebimento das informações dos servidores:

![#35](imagens/bf1942_25.png)

Vamos entender como os frames são organizados com base nas informações descobertas e apresentadas acima. Para esse exemplo, estou usando um payload já descriptografado e descomprimido pelo ```GOA``` (Gamespy Online Access):

![#36](imagens/bf1942_26.png)

Destaquei com cores para ficar mais fácil explicar o que são cada parte que compõe um payload da Gamespy.

- Iniciando pela cor vermelha, totalizando exatos 0x13 bytes, temos a Frame Signature, a assinatura do payload atual. Ela não importa muito, porém a SDK espera que sempre sejam devidamente assinados.
- Nos bytes na cor laranja claro, temos os servidores e portas organizados na estrutura: 4 bytes para IPV4 (DWORD) e 2 bytes para porta (WORD), tudo isso de maneira sequencial.
- Nos bytes na cor bege, temos a assinatura "final" que delimita o final do payload em questão.
- Tudo isso limitado a 0x7FF, que é o tamanho máximo que um payload pode possuir.

**Curiosidade, se você estiver se perguntando o porquê de o tamanho ser limitado a 0x7FF e o que acontecia se tivéssemos mais servidores do que o esperado para um payload.** A resposta é simples: porque esse é o tamanho máximo que a server browser poderia suportar, e quando tínhamos mais servidores que isso apontados para masterserver, então alguns eram pegos de maneira sortida até completar o tamanho máximo e, por fim, ser criado um novo payload e retornado ao solicitante. É exatamente por isso que causava a randomização dos servidores na server browser list, gerando oportunidades para diversos servidores aparecerem nela.

#### Escrevendo um parser para pacotes

Após conhecermos como funcionava a implementação da GameSpy e como a comunicação entre cliente e servidor ocorria, é hora de escrevermos um parser para isso! Para isso, estabelecerei alguns objetivos para nosso parser, sendo eles:

- Parsear completamente os payloads.
- Organizar informações em constantes.
- Organizar listas de servidores.
- Imprimir e obter todas as informações de um payload.
- Adicionar novos servidores.
- Remover servidores.
- Verificar status de servidores.
- Gerar novos payloads.

Iniciei pelo básico: criei um novo projeto apelidado de "EAGamesNetworkFrameParser". Nele, criei uma nova classe chamada "BF1942FrameNetParser" para armazenar todos os métodos para manipular nossos payloads.

Nesta classe, criei uma nova struct para armazenar nossos servidores, chamada de "BF1942gameServers":

```c++
typedef struct BF1942gameServers {

	/*
		IPV4
	*/
	uint32_t dwServerIP { 0 };

	/*
		PORT
	*/
	uint16_t dwServerPort { 0 };

	/*
		Data to Display on SERVER BROWSER
	*/
	std::string status_response;

};
```

Além da struct para armazenar informações dos servidores presentes no payload, criei constantes para Status-Query, End-Signature, Max-PayloadSize e Max-StatusQuery (tamanho máximo que um payload de query de servidor pode ter):

```c++
const char* m_endSignature{ "\\final\\" };
const char* m_stausQuery{ "\\status\\" };
const int MAX_STATUS_SIZE{ 0x5DB }; // MAX status size
const int MAX_PAYLOAD_SIZE{ 0x7FF }; // MAX payload size
```

Também criei declarações para dois raw pointers privados para a classe. O primeiro será utilizado para armazenar todo o buffer de um payload, ```m_ucRawFrame```. Já o segundo buffer, chamado de ```m_ucNewFrame```, armazenará todo o buffer alterado posteriormente por quem implementar a classe e desejar modificar o payload.

```c++
unsigned char* m_ucRawFrame{ NULL };
unsigned char* m_ucNewFrame{ NULL };
```

Seguindo isso, criei os seguintes métodos públicos focados nos nossos objetivos traçados:

```c++

// Construtor para a nossa classe recebendo um path para o payload a ser trabalhado
BF1942FrameNetParser( std::string& strFilePath );

// 1º Método responsável por adicionar um novo servidor ao payload
auto addNewServer( const char* chSeverIp, const char* chSeverPort, bool check=true ) -> bool;

// 2º Método responsável por remover um determinado servidor adicionado ao payload, usando o índice para isso
auto removeServer( int index ) -> void;

// 3º Método responsável por gerar um novo payload com os dados atualizados dos servidores
auto getNewPayload( ) -> unsigned char*;

// 4º Método responsável por obter o payload raw. nesse caso um raw pointer para nosso payload em questão
auto getRawPayload( ) -> unsigned char*;

// 5º Método responsável por escrever um novo payload gerado em disco, basicamente isso gerara um novo payload com os dados atualizados e então chamara rawpayload para obter um raw pointer com o buffer para então escrever seu conteúdo no path específicado
auto writeNewPayload( std::string& path ) -> bool;

// 6º  Método responsável por verificar se um servidor esta ativo e obter todas as informações básicas sobre o mesmo. para isso é necessário uma struct BF1942gameServers contendo os dados do servidor a ser verificado
auto checkServerStatus( BF1942gameServers* bfg ) -> bool;

// 7º Método operator responsável por obter uma std::string com todas as informações dos servidores presentes em um payload
operator std::string( ) const;
	
// 8º Destructor para cleanup das informações usadas por nossa classe de parser de payload
~BF1942FrameNetParser( );
```

Após essa pequena organização e padronização, iniciei a implementação do nosso parser em si. 

Ao iniciar pela lógica do construtor da classe, ele vai ser o responsável por separar e organizar os devidos procedimentos presentes no payload. Para isso, como mencionado, obtive todo o buffer do arquivo na variável privada ```m_ucRawFrame``` e a utilizei como base para trabalhar. Primeiramente, extraí a assinatura e a armazenei e avancei para o body do payload:

```c++
	
//Extraindo a assinatura do payload
std::memcpy( this->m_ucFrameSignature, this->m_ucRawFrame, sizeof( this->m_ucFrameSignature ) );

//Avançando para o início do body do payload
auto ptr = this->m_ucRawFrame;
ptr += sizeof( this->m_ucFrameSignature );
```

Após isso escrevi a lógica para extrair os servidores até que a assinatura final fosse encontrada em ```m_ucRawFrame```:

```c++

while ( std::memcmp( ptr, m_endSignature, sizeof( m_endSignature ) ) != 0 ) {

	BF1942gameServers srv{ 0 };

	std::memcpy( &srv.dwServerIP, ptr, sizeof( uint32_t ) );

	ptr += sizeof( uint32_t );

	std::memcpy( &srv.dwServerPort, ptr, sizeof( uint16_t ) );

	ptr += sizeof( uint16_t );

	this->m_servers.push_back( srv );

}
```

O código acima repete até encontrar a assinatura que delimita o final e, enquanto repete, vai extraindo cada DWORD-4 Bytes que representam um IPV4 e avançando para encontrar os 2 Bytes-WORD que representam a porta, colocando os dados coletados em uma nova instância da struct ```BF1942gameServers``` e adicionando em um std::vector.

Com isso, já temos todas as informações devidamente separadas do nosso payload original, e segui para implementar nosso operador ```std::string()```. Nessa etapa, apenas organizei os bytes referidos que representavam o IPV4 e a porta e retornei tudo organizado em uma única std::string:

```c++
for ( auto bfServer : this->m_servers )

	out << ( bfServer.dwServerIP & 0xff ) << "." << ( ( bfServer.dwServerIP >> 8 ) & 0xff ) << "."
			<< ( ( bfServer.dwServerIP >> 16 ) & 0xff ) << "." << ( ( bfServer.dwServerIP >> 24 ) & 0xff )
			<< ":" << _byteswap_ushort( bfServer.dwServerPort ) << "\n";
```

Ficando dessa forma:

![#37](imagens/bf1942_27.png)

Com esse resultado, prossegui para a implementação do método para adicionar novos servidores no nosso vector de servidores m_servers. Para isso, adotei os seguintes passos:

Primeiramente, inicializei uma nova struct ```BF1942gameServers```:

```c++
BF1942gameServers srv{ 0 };
```

Segundamente, preparei o parsing das informações a partir de um raw pointer ```const char*```, organizando os dados individualmente nos fields da struct inicializada:

```c++
int b1{ 0 }, b2{ 0 }, b3{ 0 }, b4{ 0 }, port{ 0 };

sscanf_s( chSeverIp, "%d.%d.%d.%d", &b1, &b2, &b3, &b4 );

sscanf_s( chSeverPort, "%d", &port );

srv.dwServerIP = (b4 << 24) |
		 (b3 << 16) |
		  (b2 << 8) |
		  	  b1;

srv.dwServerPort = _byteswap_ushort( port );
```

Em seguida, iniciei a implementação da lógica para verificar o status do servidor usando a query server-browser status, centralizando no procedimento ```checkServerStatus```:

```c++
.
.
if (sendto(sockfd, this->m_stausQuery, strnlen_s(this->m_stausQuery, 20), 0, reinterpret_cast<struct sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR)
.
.
recvfrom(sockfd, chBufferStatus, MAX_STATUS_SIZE, 0, reinterpret_cast<struct sockaddr*>(&serverAddr), &msForcingAPointer);
.
.
bfg->status_response = std::string(chBufferStatus);
.
.
return true;
```
Caso o servidor a ser adicionado passe na validação do status, prossigo para adicionar a instância da struct para esse servidor no vector ```m_servers```.

Não comentarei a implementação do procedimento ```removeServer``` pois ele é muito simples, já que é apenas a exclusão de um item do vector ```m_servers```.

Focando na explicação do procedimento ```getNewPayload```, ele é o responsável por gerar um novo payload do zero com todas as informações alteradas e adicionadas pelo nosso parser.

Primeiro, alocaremos um novo buffer com o tamanho máximo que o payload pode assumir:

```c++
this->m_ucNewFrame = new unsigned char[ MAX_PAYLOAD_SIZE ] { 0 };
```

Logo em seguida, copiamos a assinatura do payload:

```c++
std::memcpy( this->m_ucNewFrame, this->m_ucFrameSignature, sizeof( this->m_ucFrameSignature ) );
```

Com base em cada servidor no vector ```m_servers```, inicio a cópia para o novo payload:

```c++
for ( auto server : this->m_servers ) {

	std::memcpy( ptr, &server.dwServerIP, sizeof( uint32_t ) );

	ptr += sizeof( uint32_t );

	std::memcpy( ptr, &server.dwServerPort, sizeof( uint16_t ) );

	ptr += sizeof( uint16_t );

}
```

E após isso, incluo a assinatura que delimitará o final do payload para o parser da GameSpy SDK 2002:

```c++
std::memcpy( ptr, m_endSignature, strnlen_s( m_endSignature, 10 ) );
```

Os demais procedimentos também foram implementados por mim seguindo a mesma lógica, mas não considerei tão relevante destacá-los aqui, sendo eles respectivamente:

```
writeNewPayload
getRawPayload
~BF1942FrameNetParser
```
Consulte o código-fonte sempre para uma alta precisão na implementação.

Enfim, ao final de tudo, temos o nosso parser dos payloads completamente funcional:

![#38](imagens/bf1942_28.png)

Enfim, conseguimos vencer essa etapa, vamos adiante com nossa pesquisa.

![#39](imagens/genericanimestuff7.gif)

#### Analisando e criando ideias para modificações

Agora, vamos colocar a mão na massa e modificar a implementação da GameSpy, adicionando nosso próprio código para trabalhar com nosso próprio payload e, dessa forma, reviver a Server Browser do Battlefield 1942. Faremos isso através de um módulo apelidado por mim de ```kurumi```. A ideia é fazer algo adaptável e inteligente para cumprir com nosso objetivo.

#### Analisando padrões de código

Irei iniciar a apresentação da implementação e modificação do código da GameSpy. Vou apresentar onde e por que escolhi determinadas rotinas para sobrescrever/reescrever em uma nova região, direcionando a execução do código e mantendo assim a compatibilidade com o código original. E claro, como usei um framework super performático e estável para disassemblar e para manter a compatibilidade dessas implementações.

O projeto da Kurumi foi organizado de uma forma bem simples, contando apenas com dois arquivos, sendo um deles completamente escrito em Intel Assembly, com foco para suporte ao Masm (Microsoft Macro Assembler), evitando usar recursos novos da linguagem C++ devido a possíveis incompatibilidades que poderiam surgir com o Battlefield 1942.

Ao todo, trabalhei em apenas cinco pontos específicos da implementação da GameSpy, já considerando redirecionamento de códigos para implementações próprias ou patches muito específicos. Criei uma struct de configuração na "Kurumi" chamada ```BF1942_GS_NETWORK``` e cada campo foi nomeado com um nome mais amigável por mim:

- bf1942->pReadBuffer
- bf1942->pGamespyDecompressRoutine
- bf1942->pSetReadBufferGsReturn
- bf1942->pSetDecryptRoutineGsReturn
- bf1942->pFirstByteAddrMagicByte

O primeiro desses pontos é o ```pReadBuffer```. Originalmente, essa rotina era responsável por gerenciar os buffers lidos a partir do socket TCP estabelecido com a Master Server List Provider. A Kurumi insere um simples código de redirecionamento que armazena todo o contexto de registradores e, em seguida, o redireciona para uma área de meu próprio controle, onde reimplementei completamente a lógica original em assembly, com a única diferença de que um buffer de meu controle será utilizado. Como a ideia da implementação é ser o menos intrusiva possível, adotei essa abordagem. Veja um exemplo:

![#40](imagens/bf1942_29.png)

Esta é a rotina original da GameSpy que reescreverei em uma nova região, sem limitação para adicionar meu próprio handler do buffer a ser lido:

![#41](imagens/bf1942_30.png)

O segundo desses pontos é o ```pGamespyDecompressRoutine```. Ele é responsável por descomprimir o buffer recebido e desencriptar seu conteúdo. Porém, agora nosso buffer não requer esse recurso, então substituímos o handler original da GameSpy por um de nosso próprio controle, com as informações que foram lidas de nossa alteração anterior, ```pReadBuffer```. Tentando novamente ser o menos intrusivo possível, apenas redirecionamos a lógica para uma região com o assembly reescrito e adaptado para isso. Veja um exemplo:

![#42](imagens/bf1942_31.png)

O objetivo desta rotina já foi apresentado em tópicos anteriores. Veja a reimplementação feita:

![#43](imagens/bf1942_32.png)

Esses foram os únicos pontos que de fato foram reescritos em Assembly por mim. Os demais pontos serão destacados agora; eles simplesmente servem como delimitadores de configuração para redirecionar código entre minha implementação e a implementação original, e claro, demarcar globais usadas pela GameSpy:

- ```pSetReadBufferGsReturn``` determina o endereço de retorno de execução para pReadBuffer, onde, após executar a minha própria implementação, retorna ao código original da GameSpy.

- ```pSetDecryptRoutineGsReturn``` determina o endereço de retorno de execução para ```pGamespyDecompressRoutine```. Após executar minha própria implementação, a partir de onde a execução continuaria no código da GameSpy.

- ```pFirstByteAddrMagicByte``` armazena o endereço da global que armazena o magic byte do payload (xored com 0xEC), um byte antes de onde o payload é armazenado. O funcionamento já foi detalhado nos tópicos anteriores; essa informação foi utilizada por mim para ter um controle maior e substituir pelo meu buffer de trabalho.

**Como você abstraiu tudo isso. Espere, isso ficou muito simples, não?**

![#44](imagens/genericanimestuff8.png)

Sim, e esse de fato é o objetivo. Eu gastei um bom tempo revertendo e descobrindo como fazer de maneira efetiva. Eu queria desenvolver algo simples, prático e o menos intrusivo possível, que fosse estável e sem muitas modificações na implementação original da GameSpy. Para determinar e encontrar os padrões corretos, alterar o endereço da masterserver, em tudo que fosse possível. Para essa missão, utilizei um framework de disassembler desenvolvido pela Bitdefender, apelidado de ```bddisasm```. Você pode me questionar o porquê da minha escolha, e eu simplesmente respondo que é meu gosto pessoal. Eu acho muito leve e prático o funcionamento desse framework. Vamos entender como eu fiz:

![#45](imagens/bf1942_33.png)

A minha implementação escaneia a região correta onde as assinaturas devem bater, tudo isso de maneira otimizada e mais assertiva possível, independente da versão do Battlefield utilizada (CD ou Origin/EA Launcher), encontra o endereço correto e repete desconsiderando a região já escaneada para encontrar os demais padrões e recuperar o seu endereço virtual, armazenando-os nos devidos campos da struct ```BF1942_GS_NETWORK```.

A mesma ideia se aplica para a busca dos endereços da masterserver utilizados pelo jogo. Por exemplo, o endereço da masterserver padrão utilizado no Battlefield 1942 é ```master.gamespy.com```. Minha lógica apenas busca o endereço e o substitui por ```kotori.keowu.re```, que é a minha própria master server provider. Por padrão, o endereço da master server provider apresentado da ```gamespy.com``` era configurado a partir da própria struct de inicialização da GameSpy, e por coincidência esse endereço também é configurável no módulo Kurumi a partir do campo ```MasterServer``` da struct ```BF1942_GS_NETWORK```:

![#46](imagens/bf1942_34.png)

! Enfim, chegamos ao fim dessa apresentação da minha abordagem de substituição e reimplementação do código da SDK da GameSpy. Caso você se interesse, confira o código-fonte para maiores detalhes e curiosidade (ou até mesmo uma gameplay).

Vamos agora escrever uma nova Master Server List Provider para nosso novo módulo ```Kurumi```, baseado em todo o processo aprendido até então.

#### Escrevendo uma nova Master Server List provider

Escrever uma nova Masterserver não é uma tarefa tão complexa graças a todo o entendimento e insumos que geramos com nossa engenharia reversa e documentamos nesse artigo. Para esta tarefa, escolhi a linguagem C# com a versão do .Net Framework 8, pois é altamente portável e consigo gerar uma release que rodará tanto no Windows quanto no Linux (sim, isso é possível através da configuração de deploy. Irei explicar como fazer isso).

Organizei o desenvolvimento de uma simples Masterserver em etapas, sendo elas:

1. Criar uma classe base de suporte Socket TCP.
2. Definir um método de escuta para clientes a serem conectados.
3. Criar um método de HandleClient para cada cliente conectado no servidor.
4. Preparar o arquivo de payload para que, sempre que um novo cliente conectar ao servidor, o conteúdo seja enviado.
5. Enviar a mensagem de ```Hello``` -> ```\\basic\\secure\\MASTER``` logo no início da conexão com o cliente.
6. Receber a query do cliente solicitando o payload de servidores ```bfield1942\\final```.
7. Verificar se a query do cliente foi recebida e enviar o devido payload ao cliente.
8. Aguardar o cliente encerrar a conexão.

Não tem segredo, o código é um simples socket TCP que aceitará as conexões para a porta ```28900```, que é a mesma porta utilizada pela Gamespy.

![#47](imagens/bf1942_35.png)

O ponto principal aqui é a implementação, já que apresentamos totalmente o protocolo nos tópicos anteriores. Então aqui vamos focar em apresentar como foi implementada a comunicação entre o cliente e o servidor:

![#48](imagens/bf1942_36.png)

Quando um cliente conecta, eu obtenho o payload mais recente do binário gerado pelo projeto ```EAGamesNetworkFrameParser```, envio o comando/mensagem de apresentação de ```\\basic\\secure\\MASTER```, e em seguida inicio um loop infinito para receber o comando da query enviado pelo cliente, claramente validando qualquer exceção e tratando casos inesperados, como um cliente desconectar abruptamente. Porém, para enviar o payload de servidores, é necessário buscarmos pelo prefixo final que indica a última autenticação enviada. Ao recebê-lo, envio apenas o conteúdo do nosso payload de servidores ao cliente e encerro o socket.

Como o nosso intuito não era implementar todos os recursos oferecidos pela Gamespy, como os filtros das queries de servidor, me preocupei com o básico, que é enviar a lista de servidores corretamente da maneira que o cliente do jogo espera. A partir disso, com essa simples implementação, tudo funcionará bem!

Espere, é só isso? Isso é uma Masterserver emulator simples?

![#49](imagens/genericanimestuff9.png)

A implementação é simples quando já se tem toda a base para ela. Existe todo um trabalho de entender o protocolo e como o jogo funcionava antes de propriamente implementar todos os recursos vistos aqui neste artigo. Uma coisa leva à outra, e a cada pequeno passo temos uma grande conquista, nesse caso uma Gamespy Browser Server List completamente ao nosso dispor e controle para todos os jogos lançados e usando a SDK de 2002.

**Se você estiver se perguntando como funciona o deploy para o Linux**, ele é relativamente simples:

Gerando uma build usando o Visual Studio 2022:

- Com o projeto aberto, vá no menu ```Build``` > ```Publish Selection```.
- Selecione ```Folder``` nas opções do diálogo aberto, pressione next e repita o mesmo processo.
- Defina o diretório a ser salvo o build para Linux e clique em ```Finish```.
- Clique na opção ```Show All Settings``` da publish aberta. Clique no select box da ```Target Runtime``` e selecione ```Linux-x64```, marque a opção ```Produce Single File``` e clique em ```Salvar```.
- Por fim, clique no botão ```Publish``` e apenas aguarde até a solução ser compilada.
- Vá até o diretório, copie o binário ELF gerado e avance para preparação do seu Linux em questão.

Preparando ambiente no Linux:

- Não detalharei o processo de instalação do Dot Net Runtime 8.0 (não existe segredo, o próprio binário de resultado vai lhe fornecer instruções para download).
- Defina as variáveis de ambiente: ```export DOTNET_ROOT=/home/ubuntu/gamespymasterserver/dotnet8.2```, ```export PATH=$PATH:/home/ubuntu/gamespymasterserver/dotnet8.2```.
- Libere as devidas portas no seu firewall: ```sudo firewall-cmd --add-port=28900/tcp```.
- Rode o binário compilado: ```./GameSpyMasterServer &```.

- ! Dependendo da sua plataforma de hospedagem, é necessário configurar a porta TCP pelo painel do provedor.

Veja como ficou nossa Masterserver plenamente funcionando em conjunto com o projeto todo:

GameSpy Master server esperando novos clientes:

![#50](imagens/bf1942_37.png)

Um novo cliente conectou e desconectou logo em seguida:

![#51](imagens/bf1942_38.png)

Enquanto isso, do lado do cliente, todos os servidores são renderizados com sucesso com base no payload de servidores gerados por nós:

![#52](imagens/bf1942_39.png)

#### Testando o projeto

Vamos testar agora os resultados obtidos com nossa pesquisa. Aqui vamos ter demonstrações em vídeos no YouTube de tudo em perfeito funcionamento:

**DEMO 1(Dev showing the project):**

[![KewGamespy - Battlefield 1942 DEMO 1](https://img.youtube.com/vi/VOKm6LyfC30/0.jpg)](https://www.youtube.com/watch?v=VOKm6LyfC30)

**DEMO 2(Small Gameplay):**

[![KewGamespy - Battlefield 1942 DEMO 2](https://img.youtube.com/vi/XSBIUQTtwIg/0.jpg)](https://www.youtube.com/watch?v=XSBIUQTtwIg)


## A Gamespy de 2004 (Usada pela Bungie e EA)

A implementação da GameSpy 2004 utilizada e implementada pela Bungie e EA em seus jogos possui algumas grandes diferenças se comparada à versão de 2002 utilizada pelo Battlefield 1942. Vamos compreender cada detalhe de como ela funciona desde o momento do recebimento dos payloads, implementação até o parsing e finalizando com a escrita completa da MasterServer de nosso controle. Além disso, corrigiremos os jogos que a implementam para que funcionem através de nossa nova MasterServer provider de nosso controle.

![#53](imagens/genericanimestuff8.gif)

#### Um pouco do posicionamento da EA Games x Bungie

Antes de continuarmos a estudar a GameSpy, vamos entender um pouco de como foi o posicionamento das empresas em relação a salvar seus títulos logo após o fechamento da GameSpy em 2014. Como já era de se esperar, a EA Games fez seu movimento mais clássico em simplesmente não agir e deixar seus títulos morrerem. Já a Bungie mostrou um carinho pelos seus títulos ao obter o código fornecido pela IGN na época e hospedá-los por conta própria em uma instância na AWS-EC2, no domínio ```*.hosthpc.com```:

![#54](imagens/bungie_40.png)

Então, se atualmente você ainda tiver o Halo CE, pode simplesmente aplicar um patch disponibilizado pela própria Bungie e continuar jogando sem impacto algum. Porém, ao decorrer desse artigo, usaremos o Halo e também ofereceremos suporte, dessa forma garantindo que ele jamais venha a morrer caso a Bungie resolva desligar seus servidores como a EA.

### Como ocorre a comunicação de rede

As seguintes portas são utilizadas pela SDK da Gamespy 2004:

1. **28910** - Toda a comunicação entre o MasterServer e o Jogo ocorre através desta porta utilizando o protocolo TCP.
2. **2302[Totalmente configurável]** - Assim como na Gamespy 2002, os servidores e seus status são verificados através do protocolo UDP, com a única diferença de que essa porta é totalmente configurável no próprio payload dos servidores.
3. As demais comunicações ocorrem normalmente através do protocolo TCP.

#### Analisando os Pacotes

Vamos entender como isso ocorre na prática. Para isso, utilizarei a minha reimplementação do MasterServer da Gamespy 2004. Para simplificarmos o entendimento de como ocorrem as comunicações, vamos separar por detalhes iniciando desde a comunicação da query da Gamespy, seu conteúdo, a engenharia reversa da implementação da SDK e, por fim, do payload recebido.

**Iniciando uma nova browsing request**

Uma browsing request é feita quando você efetua alguma interação com o botão ```Get List``` presente nos jogos que implementam a Gamespy como um todo. Na sua versão 2004, não seria diferente. Vamos iniciar uma nova request para capturar os dados trafegando. Usarei o Halo CE nesse exemplo, porém, obviamente, você pode usar outros jogos. No futuro, repeti os passos com o Battlefield Vietnam. Veja como foi feito:

![#54](imagens/bungie_41.png)

! Após clicar no botão de requesting, vamos analisar as informações que conseguimos obter.

**A Sever Browser Auth**

Se você estiver se perguntando se não existe mais a etapa de autenticação que conhecemos quando introduzimos a Gamespy 2002, esta etapa foi removida na Gamespy 2004.

**A server browser query**

Assim que conectado ao socket TCP da MasterServer provider, a SDK envia a query para o jogo em questão:

![#55](imagens/bungie_42.png)

Se comparado com a Gamespy 2002, percebemos algumas pequenas mudanças no frame da query em questão, sendo apenas o nome duplicado do jogo:

```
halor\halor\hostname\gamever\numplayers\maxplayers\mapname\gametype\password\dedicated\game_classic\teamplay
```

Vamos entender o que significa cada componente dessa query:

- **halor\halor** - Jogo em questão no qual deseja-se obter payload de servidores. O valor dessa query é completamente definido e atribuído pela Gamespy aos desenvolvedores.
- **hostname, gamever, numplayers, maxplayers, mapname, gametype, gametype, password e dedicated** - Informam quais informações o payload deverá conter. Isso é conhecido como keys quando referenciadas internamente pelos jogos que utilizam a SDK. O payload é totalmente orientado a elas. Ou seja, a SDK de outro jogo que implementa a Gamespy pode ou não conter os mesmos fields ou até mais fields, dependendo, é claro, do próprio desenvolvedor que as implementa.
- **game_classic, teamplay** - São obrigatórios em qualquer query da Gamespy 2004 e simplesmente são os filtros básicos que todo desenvolvedor deve implementar utilizando a callback da Gamespy.

**O Serverbrowser Payload**

De maneira similar à versão 2002 da SDK, a SDK 2004 também implementa o sistema de payloads criptografados e comprimidos com o algoritmo proprietário GOA (Gamespy Online Access), obviamente com a estrutura completamente diferente, a qual detalharemos completamente ao decorrer de nossa pesquisa.

Logo após a query ser enviada para o MasterServer provider, o mesmo gera o payload e responde de volta para o cliente Gamespy:

![#56](imagens/bungie_43.png)

Diferente da SDK 2002, nosso payload tem um tamanho até duas vezes maior de capacidade se comparado com a sua versão anterior. Ou seja, podemos ter um buffer completo de tamanho máximo de ```4096``` bytes. No entanto, os conceitos passados de randomização de servidores a cada nova query ainda ocorrem normalmente. Ou seja, mesmo que tenhamos um número muito superior de servidores fazendo heartbeat no MasterServer provider, ainda temos um fator randômico que os atribui sempre de maneira que todos os servidores online tenham o mesmo destaque na lista de server browsing.

Devido ao tamanho dos payloads, os mesmos são divididos em duas partes, com ambos se completando:

![#57](imagens/bungie_44.png)

![#58](imagens/bungie_45.png)

Antes de irmos a fundo em como o payload é composto, vamos entender como ele é parseado, dessa vez direcionando nossos estudos no código revertido da implementação da Gamespy 2004.

**Revertendo a implementação de server browsing da Gamespy 2004 para entender o seu funcionamento**

Vamos analisar o processo de trabalho com os payloads recebidos do MasterServer provider pela GS2004. Durante a análise, reverti completamente o funcionamento do código que tem esse trabalho de receber, organizar, parsear keys e flags dos payloads e, claro, os servidores. Os dois procedimentos importantes e responsáveis por todo o trabalho são ```get_socket_gamespy_buffer``` e ```execute_tcp_decryption```, ambos interligados e dependentes em seu funcionamento.

Antes de irmos a fundo, precisamos ter uma pequena base de conhecimento em como são gerenciadas as informações de conexão na SDK. Temos diversas checagens de status e integridade da conexão e flags de status, e callbacks registradas por quem implementa a SDK. Então, faz-se necessário entendermos a struct base de configuração de sockets de rede, aqui referidas como ```gamespysocket``` ou ```clsGSCon```, com a seguinte declaração revertida:

```c++
struct gamespysocket {
  UINT32 connection_status_flag;
  . . .
  UINT32 pGamespyBuffer;
  UINT32 actual_size;
  . . .
  void (__cdecl *throw_gamespy_connection_error)(UINT32, UINT32, UINT32, UINT32);
  UINT32 state;
  . . .
  UINT32 socket;
  UCHAR goa_header_key[276];
  UINT32 gs_state;
};

```
Esta struct é importante e utilizada para armazenar todos os estados e configurações referidos às conexões feitas pela SDK da Gamespy 2004. Cada field possui um significado que vamos detalhar neste momento:

- ```connection_status_flag``` - Armazena flags de estado de conexão atualmente assumido pela SDK. Flags como: conectado, desconectado, socket error, timeout, entre outras.
- ```pGamespyBuffer``` - Armazena a referência ao payload lido a partir do socket estabelecido com a Gamespy.
- ```actual_size``` - Armazena informações referentes ao tamanho do payload armazenado. Esse campo é extremamente volátil e variável, então nem a própria SDK o considera informação confiável (já que, ao decorrer da explicação, entenderemos como de fato fazem a checagem pelo tamanho).
- ```throw_gamespy_connection_error``` - Armazena o endereço da função de callback para erros de conexão definidos por quem implementa a SDK.
- ```state``` - Estado utilizado para indicar o estado de onde ocorreu determinada Gamespy connection error, basicamente o motivo de ter ocorrido. Diferente da connection_status_flag, que é interna, este valor é retornado ao callback registrado pelo desenvolvedor.
- ```socket``` - É uma referência SOCKET convencional estabelecida usando a API do sistema operacional ao qual foi compilada (isso varia entre sistemas operacionais, já que a Gamespy é multiplataforma).
- ```goa_header_key``` - É a chave de criptografia para os headers da conexão obtidos a partir do primeiro payload de conexão diretamente do header do payload.
- ```gs_state``` - Armazena erros de estado do próprio payload ou configurações customizadas, como informações de flags inválidas, geralmente só ocorrendo quando uma configuração de SDK é feita inválida ou um payload de outro jogo é recebido ao invés da flag do jogo atual.

Após essa breve introdução, podemos começar a destrinchar o código revertido do procedimento ```get_socket_gamespy_buffer```. Este procedimento é responsável por verificar se existem conteúdos a serem recebidos no socket TCP ativo estabelecido com a MasterServer da Gamespy, além de verificar a integridade do socket, bem como as ```connection_status_flag``` que indicam estado de erro na conexão:

![#59](imagens/bungie_46.png)

Após as devidas verificações de integridade, ocorre a leitura do payload do socket estabelecido com a MasterServer. Esse payload pode ter o tamanho máximo de ```4096 bytes```. Além de termos checagens de integridade para garantir que tudo foi devidamente recebido: ```if ( readed_size == -1 || !readed_size )``` e nossa primeira chamada para a callback de desenvolvedor informando o erro de conexão ```clsGSCon->throw_gamespy_connection_error(clsGSCon, 4, g_nullserverref, clsGSCon->state);```, caso tudo ocorra bem, avançamos para os próximos estágios, de calcular o tamanho do payload recebido e checagens por casos específicos de funcionamento da SDK:

![#60](imagens/bungie_47.png)

Porém, a partir de agora, com um buffer já lido e válido, podemos observar a chamada para ```execute_tcp_decryption(clsGSCon)```, o segundo procedimento mais importante de todos que será de fato o responsável por parsear, descriptografar, descomprimir, validar, separar flags e servers e chamar a callback para popular a Server Browser implementada pelo desenvolvedor do jogo. No entanto, antes de partirmos para destrinchar o seu funcionamento em detalhes, vamos compreender como funciona a estrutura ```config_gamespy```, interna da própria SDK, sem acesso de quem a implementa, no entanto, muito importante para o funcionamento da mesma.

```c++
struct config_gamespy {
  UINT32 state;
  . . .
  UINT32 keylist;
  . . .
  PUCHAR pDecryptedBuffer;
  UINT32 pDecryptBufferIn;
  UCHAR values_to_populate[1020];
  UINT32 num_populate;
  UINT32 expectedelements;
  void (__cdecl *ListCallBack)(UINT32, UINT32, UINT32, UINT32);
  UINT32 instance;
  . . .
  UINT32 ip_usuario_placed_requested;
  . . .
  UINT32 defaultPortGameservers;
  UINT32 callback_body_init;
  . . .
  UCHAR crypt_key;
  . . .
  UCHAR gs_query_flags;
  UINT32 pstate;
};
```

- ```state``` - Com funcionamento similar à estrutura gamespysocket, armazena códigos de erros e estado de processamento do parsing do payload de configuração dos servidores, utilizado apenas internamente na SDK.
- ```keylist``` - Armazena uma referência para as keylists do jogo atual, basicamente strings de entradas e configurações e modos de jogo a serem exibidas no server browser.
- ```pDecryptedBuffer``` - Buffer do payload já descriptografado e descomprimido armazenado após o processamento do GOA (Gamespy Online Access). Esse buffer é muito importante, pois será utilizado para trabalho e parseamento de servidores, keys e tags.

- ```pDecryptBufferIn``` - Tamanho do buffer do payload total, sendo atualizado conforme se é trabalhado e parseado.

- ```values_to_populate``` - Carrega uma lista com os servidores e suas portas definidas no payload já parseados individualmente.

- ```num_populate``` - Número de servidores na lista a serem populados por meio da ListCallBack registrada pelo desenvolvedor que implementou a SDK.

- ```expectedelements``` - Tamanho de elementos de keys esperados por quem implementou a SDK, isso varia de cada jogo.

- ```ListCallBack``` - A callback registrada pelo desenvolvedor em questão que implementou a SDK, onde receberá uma lista ligada de servidores e suas devidas keys, já validadas.

- ```instance``` - Instância da server list que armazenará os dados a serem armazenados posteriormente.

- ```ip_usuario_placed_requested``` - Endereço IP do usuário que solicitou a lista de servidores para a Masterserver List.

- ```defaultPortGameservers``` -  A porta padrão, caso a porta padrão que acompanha cada servidor no payload esteja 0x0000 ou 0xFFFF, esse valor será utilizado e assumido ao obter detalhes do servidor.

- ```callback_body_init``` - Ponteiro para o início exato de onde começam os dados do payload (pulando o cabeçalho que o acompanha).

- ```crypt_key``` - Chave de criptografia a ser utilizada pelo algoritmo do GOA no processo de decrypt.

- ```gs_query_flags``` - Flags utilizadas pela Gamespy Masterserver durante a consulta de servidores.

- ```pstate``` - Apenas define o status da chave de criptografia como iniciada. 1 - Chave de criptografia definida a partir do header, e 0 - Sem chave de criptografia definida a partir do header.

Após toda essa introdução, podemos enfim ir direto ao segundo procedimento mais importante para compreendermos a GameSpy 2004: o ```execute_tcp_decryption```.

![#61](imagens/bungie_48.png)

Logo no início do procedimento, podemos ver as lógicas responsáveis por obter o tamanho do payload e obter uma referência do payload armazenado na struct de conexão lida anteriormente por ```get_socket_gamespy_buffer```. O objetivo aqui é obter o offset do byte de assinaturas usado para obter a chave do body do payload do GOA ```signature_offset = (*decompressed_stack_buffer ^ 0xEC) + 2;```. Esse valor jamais pode ser inferior ao tamanho total do payload. Esse XOR não é à toa, e nem a soma com 2. Ele está basicamente descriptografando a informação do header. Com base nesse offset, temos a primeira utilização do valor ao realizar um acesso ```keyLen = decompressed_stack_buffer[signature_offset - 1] ^ 0xEA;``` com o intuito de recuperar o tamanho da chave de criptografia do body. Esse valor também é criptografado com um XOR. Com o segundo uso, calcula-se o offset do início do body do payload em ```bufferReadedBytes[0] = keyLen + signature_offset;```. Esse valor será utilizado para descartar todos os valores do cabeçalho GOA e, posteriormente, trabalhar apenas com o body do payload. O algoritmo responsável por descriptografar e inicializar a chave ocorre com a chamada para ```decompress_one(this, (int)&decompressed_stack_buffer[signature_offset], keyLen);```. Não temos o intuito de reverter o GOA e suas funcionalidades, então o substituiremos completamente posteriormente, aumentando também a velocidade de processamento do payload vindos de nossa MasterServer paralela. Logo após a etapa de inicialização do GOA, temos o recálculo do deslocamento do payload ao body ```inlen -= bufferReadedBytes[0];``` e ```decompressed_stack_buffer += bufferReadedBytes[0];```, além de um field muito importante ser definido: ```this->pstate = 1;```, apresentado anteriormente como controle de que a GOA e a chave foram inicializadas devidamente. Concluímos essa primeira etapa com o decrypt completo do body do payload em ```decompress(&this->crypt_key, decompressed_stack_buffer, inlen);```, a parte mais importante, porque agora os dados verdadeiros estarão visíveis para trabalho.

![#62](imagens/bungie_49.png)

Logo em seguida, podemos perceber que o field instance é recuperado ```serverlist_instance = this->instance;```, porque a primeira callback da Gamespy será efetuada ao desenvolvedor posteriormente. Os valores do endereço IP do requester do payload também são recuperados ```this->ip_usuario_placed_requested = *reinterpret_cast<UINT32 *>(decompressed_stack_buffer);```. Após o parsing do endereço IP, esses dados são apresentados ao desenvolvedor que implementou a callback ```this->ListCallBack(this, 6, g_nullserverref, serverlist_instance);```, cabendo a ele decidir se utilizará os mesmos. Logo após essa primeira callback, temos o parsing da porta padrão ```standardPortDefinedOnHeader = *reinterpret_cast<UINT32*>(decompressed_stack_buffer + 2);``` que será armazenada na struct de configuração interna ```this->defaultPortGameservers = standardPortDefinedOnHeader;``` como mencionado anteriormente, esses dados são usados quando a porta do servidor em questão não está disponível, durante a checagem de heartbeat via protocolo UDP. Algumas restrições se aplicam a esse valor padrão, nunca podendo ser ```0xFFFF```, significando que o parsing será abortado imediatamente por erro de configuração por parte do desenvolvedor. Seguido a isso, temos cálculos de tamanhos e offsets até chegarmos na primeira parte mais interessante:

![#63](imagens/bungie_50.png)

Esta rotina é responsável por extrair cada item-key do payload. As item-keys são representadas como caracteres ASCII válidos e fazem parte da configuração da estrutura da server browser list, ao qual a SDK estruturará as informações para que o desenvolvedor as utilize. A extração ocorre ao mesmo tempo que o offset atual do payload é atualizado ```decompressed_stack_buffer += frame_advance;```. O tamanho da seção que armazena as item-keys no body é utilizado nesta interação ```this->expectedelements > reupdated_populated_num```. Porém, não focaremos a fundo nesse tópico, pois essa etapa é customizada para cada jogo e não se altera entre novos payloads.

![#64](imagens/bungie_51.png)

Avançando um pouco mais durante a análise, chegamos finalmente na etapa onde os servidores são parseados a partir do payload, além, é claro, de atualizar os desenvolvedores que implementaram a SDK antes de iniciar a verificação UDP ao final do processamento dos servidores no payload. O procedimento ```task_list_parse_server``` tem a função de parsear cada servidor individualmente e adicionar em uma lista para posterior verificação.

![#65](imagens/bungie_52.png)

O procedimento em questão tem a função de validar o bloco atual de servidores baseado no byte da flag de status. As flags são totalmente customizadas pelo desenvolvedor e indicam o deslocamento a seguir a ser somado para acessar a próxima flag com o bloco do próximo servidor em questão. O esquema de flag pode ser utilizado para armazenar outras informações que o desenvolvedor deseje incluir nos blocos de servidores. Assim que as flags são validadas, o procedimento checa para ver se já atingiu o final do payload verificando uma constante DWORD ```LAST_SERVER_MARK_BYTES(0xFFFFFFFF)```. Esse valor mágico indica que não existe mais nenhuma informação e que o final foi atingido, encerrando assim o parsing de servidores. Do contrário, enquanto essa condição não for cumprida, continuamos com o parsing dos servidores. O procedimento ```parse_server_ip_port``` é responsável por extrair o IP ```DWORD(4 bytes)``` e a porta do servidor ```WORD(2 bytes)```, seguido de uma alocação de uma nova entrada para a lista de servidores ```alloc_server_item_for_list``` incluindo as informações parseadas. Seguido disso, temos duas verificações de sanidade para a nova lista criada, sendo a primeira uma verificação se o procedimento de alocação funcionou corretamente ```isInvalidServer``` e a segunda para calcular o offset da próxima entrada de servidores e início da próxima flag ```parse_server_validate```, concluído com a nova entrada inclusa na lista de servidores ```server_list_append_server``` que será retornado na callback registrada pelo desenvolvedor após a conclusão do parsing.

**Se você estiver questionando sobre a analise para o Battlefield Vietnam ?**

Os padrões são exatamente os mesmos (tanto para o Halo CE da análise acima, quanto para o Battlefield Vietnam). Não há diferença de implementação entre as SDKs nos jogos, apenas nas configurações usadas por eles. Ou seja, tudo que apresentamos aqui pode ser aplicado em qualquer jogo utilizando a SDK da Gamespy 2004.

Vamos aplicar o que aprendemos anteriormente e analisar um payload completo identificando cada componente que o compõe (para este exemplo, escolhi o Battlefield Vietnam):

![#66](imagens/bungie_53.png)

- Na cor roxa temos o header do payload - 37 bytes.
- Na cor vermelha temos o endereço IP do requester para o MasterServer (00000000) - 4 bytes.
- Na cor verde claro temos a porta padrão a ser utilizada durante a verificação de heartbeat via protocolo UDP (59D8) - 2 bytes.
- Na cor verde temos as Item-Keys discutidas anteriormente, que determinam as queries e como é organizado o server browser totalmente customizado pelo desenvolvedor - 192 bytes ou mais.
- Em seguida, com a cor verde escuro, temos os blocos de flags e servidores: 0x15 representando a flag de informação padrão, 4 bytes representando a representação hexadecimal de um endereço IPV4, e 2 bytes representando a porta de conexão (seguindo as regras já apresentadas anteriormente).
- Finalizando com a cor azul, representando o delimitador final de parsing do payload.

Enfim, chegou o momento de replicar tudo isso para manipular e criar nossos próprios payloads de servidores agora.

![#66](imagens/bungie_54.jpg)

#### Escrevendo um parser de pacotes para Gamespy 2004

Vamos agora reimplementar totalmente o parser para a GameSpy 2004 com base nos nossos conhecimentos adquiridos com a análise. Durante o desenvolvimento do parser, foquei em separar lógicas diferentes para o Halo CE e Battlefield Vietnam, porém o conceito permanecerá sempre o mesmo para essa versão da GameSpy.

Nesta demonstração, utilizarei o parser do Battlefield Vietnam; no entanto, o código-fonte do parser do Halo CE é muito similar, e tudo isso estará disponível para consulta e pesquisas futuras no repositório do projeto. Colocarei algumas metas que devemos cumprir ao final deste tópico:

1. Parsear completamente os payloads.
2. Organizar as informações do payload de maneira concisa.
2. Adicionar e remover servidores
3. Alterar requester IP.
4. Exibir todos os servidores e informações pertinentes sobre os payloads.
5. Gerar um novo payload com informações alteradas.

Organizei o projeto dentro do próprio projeto ```EAGamesNetworkFrameParser```, o mesmo já usado ao demonstrar o parser para a GameSpy 2002. Separei a lógica em dois arquivos: ```BFVietnamFrameNetParser.cc``` e ```BFVietnamFrameNetParser.hh```.

No arquivo de cabeçalho, organizei as informações de configurações e estrutura da classe e criei uma estrutura para organizar os endereços IP e portas de servidores, ```BFVietnamgameServer```:

```c++
typedef struct BFVietnamgameServer {

  /*
    Server IP
  */
  uint32_t dwServerIP;

  /*
    Server Port
  */
  uint16_t wServerPort;

};
```

Essa estrutura será populada e armazenada em um vector para cada um dos servidores presentes no payload parseado.

```c++
/*
 List with all servers and new servers into a payload
*/
std::vector<BFVietnamgameServer> m_BfVietnamServers;
```

Agora, em relação às informações de endereço IP do requester para a GameSpy e Porta Padrão dos servidores, tive a abordagem de armazená-los em fields dedicados:

```c++
/*
  Requester IP Address
*/
uint32_t m_dwRequesterIP{ 0 };

/*
  Default Gameservers query port
*/
uint16_t m_dDefaultPort{ 0 };
```

Além disso, em relação às constantes e valores padrões para as flags utilizadas pela SDK, também criei fields dedicados para cada um, para que possamos trabalhar de uma forma mais tranquila e organizada posteriormente:

```c++
/*
  New flag delimiter flag
*/
const unsigned char m_ucServerDelimiterFlag{ 0x15 };

/*
  Payload signature flag
*/
const unsigned char m_frameEndSignature[5]{ 0x00, 0xFF, 0xFF, 0xFF, 0xFF };

/*
  Max payload size per server browser query
*/
const int m_MAX_PAYLOAD_SIZE{ 4096 };
```

Se você estiver se perguntando como ficaram os métodos desta classe, eles ficaram desta forma:

```c++

//1º Construtor da classe que sera responsável por receber um arquivo do payload, extrair todas as informações e organiza-las.
BFVietnamFrameNetParser(std::string& strFilePath);

//2º Método responsável por obter o endereço IP do autor da requisição para a MasterServer e retorna-lo formatado no padrão IPV4.
auto getRequesterIp() -> std::string;

//3º Método responsável por definir uma novo endereço IP para o requester da MasterServer.
auto setRequesterIp(const char* chIpv4) -> void;

//4º Método responsável por adicionar um novo servidor ao payload, para isso recebendo como entrada um endereço IPV4 e uma porta
auto addServer(const char* chIpv4, const char* chPort) -> void;

//5º Método reponsável por remover um servidor do payload baseado no índice dele na lista de servidor
auto deleteServer(int index) -> void;

//6º Método responsável por retornar o novo payload com base no contexto atual da classe do payload em memória
auto getNewPayload() -> unsigned char*;

//7º Método responsável por retornar o payload raw já formatado com estado atualizado em memória
auto getRawPayload() -> unsigned char*;

//8º Método responsáevl por gravar o payload atualizado em disco
auto writeNewPayload(std::string& path) -> void;

//9º Método operator responsável por obter uma std::string com todas as informações dos servidores presentes em um payload
operator std::string() const;

//10º Destructor para cleanup das informações usadas por nossa classe de parser de payload
~BFVietnamFrameNetParser();
  
```

Escolhi alguns métodos para comentar de maneira macro e entender como eles funcionam, sem se aprofundar muito tecnicamente sobre como atuam. Para isso, recomendo que você **baixe o código e faça seus próprios experimentos.**

Iniciando pelo construtor ```BFVietnamFrameNetParser```, responsável por parsear cada parte do payload:

```c++

//Extraindo o header do payload, apelidado aqui por mim de "payloadSignature".
std::memcpy(this->m_payloadSignature, pSeek, sizeof(this->m_payloadSignature));

//Avançando pelo tamanho do offset do header
pSeek += sizeof(this->m_payloadSignature);

//Extraindo o endereço IP do requester para a GameSpy
std::memcpy(&this->m_dwRequesterIP, pSeek, sizeof(this->m_dwRequesterIP));

//Avançando pelo tamanho do offset do IP do requester
pSeek += sizeof(this->m_dwRequesterIP);

//Extraindo a porta padrão definida pelo desenvolvedor que utilizou a SDK da GameSpy
std::memcpy(&this->m_dDefaultPort, pSeek, sizeof(this->m_dDefaultPort));

//Avançando pelo tamanho do offset da porta
pSeek += sizeof(this->m_dDefaultPort);

//Extraindo o Item-keys do payload
std::memcpy(this->m_payloadGameConfiguration, pSeek, sizeof(this->m_payloadGameConfiguration));

//Avançando o tamanho do Item-keys + 1 byte para a primeira flag do bloco de servidores
pSeek += sizeof(this->m_payloadGameConfiguration) + 1; // +1 for ignore (magic flag 0x15) the first server delimiter

//Parseando o bloco dos servidores e flags, até encontrar o magic final.
while (std::memcmp(pSeek, m_frameEndSignature, 5) != 0) {

        //Verificando se temos alguma flag de delimitador de servidor sendo utilizado, caso tenha avançaremos +1 byte para obter o início do próximo bloco.
	if (std::memcmp(pSeek, &this->m_ucServerDelimiterFlag, 1) == 0) pSeek += 1;

        //Iniciando uma nova esturura para armazenarmos os dados dos servidores extraídos.
	BFVietnamgameServer bfVs;
	
        //Parseando o endereço IP do servidor
	std::memcpy(&bfVs.dwServerIP, pSeek, sizeof(bfVs.dwServerIP));
	
        //Avançando para o próximo offset
	pSeek += sizeof(bfVs.dwServerIP);

        //Parseando a porta do servidor
	std::memcpy(&bfVs.wServerPort, pSeek, sizeof(bfVs.wServerPort));

        //Avançando para o próximo offset
	pSeek += sizeof(bfVs.wServerPort);

        //Armazenando a estrutura com os dados no nosso vector final, para posterior uso.
	this->m_BfVietnamServers.push_back(bfVs);

}
```

O código acima é responsável por parsear cada um dos dados, a primeiro momento, o header do payload, seguido do endereço IP do requester, a porta padrão dos servidores, seguido do parser das Item-Keys definidas como configuração específica para o jogo em questão e finalizando com o parser total de cada um dos servidores e tags, até encontrar a assinatura final do payload com o magic ```0xFFFFFFFF```.

O método ```writeNewPayload``` é responsável por fazer o processo inverso do nosso construtor. Ele criará um novo payload para armazenar todas as alterações, fazendo exatamente o processo reverso de um parser, adicionando cada bloco como: cabeçalho, IP requester, porta padrão, Item-Keys e finalizando com os blocos de servidores e suas devidas tags e com o Magic ao final, gravando tudo isso em um arquivo em disco para ser utilizado pela MasterServer Provider.

Além do construtor e da lógica do parser apresentado, a lógica dos demais procedimentos se parece muito com o que vimos anteriormente no tópico da GameSpy 2002. Sendo assim, por já termos visto antes, resolvi pular a explicação dos seguintes métodos:

```c++
auto getRequesterIp() -> std::string;
auto setRequesterIp(const char* chIpv4) -> void;
auto addServer(const char* chIpv4, const char* chPort) -> void;
auto deleteServer(int index) -> void;
auto getNewPayload() -> unsigned char*;
auto getRawPayload() -> unsigned char*;
operator std::string() const;
```

**! No entanto, mais uma vez cabe destacar que eles se encontram disponíveis para consulta no próprio repositório do projeto.**

Após escrevermos nosso parser para a GameSpy 2004, chegou enfim o momento de começarmos a corrigi-lo com base nos conhecimentos que adquirimos, de maneira que tenhamos total controle sobre a SDK.

![#67](imagens/bungie_55.png)

#### Analisando e criando ideias para modificações GameSpy 2004

Agora, de maneira similar ao feito para a GameSpy 2002, vamos colocar a mão na massa e reimplementar as lógicas de recebimento de payloads da MasterServer Provider original para aceitar nossos próprios payloads e renderizá-los na server browser list dos jogos que implementam a GameSpy 2004. O novo módulo para essa tarefa foi apelidado de ```kuromi```, e seu objetivo é modificar de maneira eficaz e otimizada toda a lógica e os handlers para nosso próprio domínio e controle.

#### Analisando padrões de código da GameSpy 2004

Para modificar a SDK da GameSpy, criei um novo projeto apelidado de ```Kuromi```, desenvolvido em C++ e Microsoft Macro Assembly. Este módulo é capaz de identificar padrões de código utilizados pela SDK e redirecionar a execução de instruções originais para rotinas de nosso próprio controle, bem como modificar todos os endpoints utilizados pelo MasterServer Provider original para nossos próprios, de forma que possamos completamente emular o funcionamento dos componentes da GameSpy ao nosso próprio dispor, algo um pouco similar ao que aprendemos no artigo com o módulo ```kurumi```, porém sem muitas limitações impostas pelo jogo antigo da EA.

Kuromi é um módulo extremamente simples e inteligente, utilizando apenas os seguintes novos procedimentos para trabalho:

- new_get_socket_gamespy_buffer_gs2004_stub
- new_goa_decrypt_buffer_gs2004_stub

Com apenas esses dois procedimentos, reimplementei a lógica completa dos procedimentos originais utilizando Macro Assembly.

```new_get_socket_gamespy_buffer_gs2004_stub```:

![#67](imagens/bungie_56.png)

```new_goa_decrypt_buffer_gs2004_stub```:

![#68](imagens/bungie_57.png)

Além das rotinas novas modificadas, também aplicamos todas as estruturas revertidas para poder manipular os dados e informações da SDK de uma maneira muito mais eficiente no código x86:

![#69](imagens/bungie_58.png)

Similar ao módulo ```kurumi```, este módulo também é capaz de escanear os padrões utilizados pela GameSpy de maneira a encontrar os endereços corretos para redirecionar a execução do código original:

![#70](imagens/bungie_59.png)

Esse escaneamento acontece apenas no range da seção de código executável e, conforme os endereços são encontrados, são armazenados na estrutura de configuração ```GS2004_NETWORK```, para posterior redirecionamento.

Além da busca pelos procedimentos a serem redirecionados, temos outra rotina muito importante, responsável por recuperar o field ```MasterServer``` da ```GS2004_NETWORK``` e utilizá-lo para alterar os endpoints originais da SDK pelos de nosso controle:

![#71](imagens/bungie_60.png)

E com essas rotinas concluímos completamente as modificações necessárias para corrigir a GameSpy 2004 e redirecioná-la para nosso próprio controle.

![#72](imagens/bungie_61.png)

Você deve estar se perguntando, similar ao GameSpy 2002, se isso ficou muito simples. No entanto, esse de fato é o objetivo. Não há por que ser complicado. Nosso objetivo desde que começamos a reverter a SDK era compreender de uma maneira profunda seu funcionamento, e essa dedicação acaba refletindo na qualidade das manipulações que fazemos nela. Por isso, um código tão simplificado, estável e seguro.

Enfim, vamos à nossa parte final: reescrever nossa MasterServer Provider para termos tudo efetivamente em funcionamento.

#### Reescrevendo a MasterServer Provider para a GameSpy 2004

Para atender e suportar a GameSpy 2004, adaptei o mesmo código que anteriormente vimos no trecho que estudamos da GameSpy 2002. A diferença principal aqui é que vamos suportar a conexão através de duas portas TCP diferentes, sendo assim, trabalhei com dois sockets em vez de um único socket:

![#73](imagens/bungie_62.png)

O funcionamento base do MasterServer Provider foi um pouco modificado. Uma TASK é criada para cada um dos sockets, e o socket que responder de maneira assíncrona gerencia a conexão recebida, direcionando ao devido procedimento com o prefixo ```HandleGSxxxxClients```:

![#74](imagens/bungie_63.png)

E, de maneira similar ao que já vimos na GameSpy 2002, respondemos a solicitações vindas de nosso cliente da maneira que a SDK necessita e, ao final de tudo, enviamos o payload completo para o cliente:

![#75](imagens/bungie_64.png)

Neste exemplo, quando um novo cliente conectar pela porta da GameSpy 2004, verificaremos se a query contém "Battlefield Vietnam" ou "Halo", e, com base nisso, apenas respondemos o devido payload de configuração gerado com nosso algoritmo de parser anteriormente explicado. De maneira similar, isso ocorre também com a GameSpy 2002:

![#76](imagens/bungie_65.png)

No entanto, como já sabemos, ela requer muito mais interações e possui muito mais queries para configurações do payload, algo que a GameSpy decidiu alterar na versão de 2004.

Não ensinarei o processo para deploy do MasterServer Provider aqui, pois já foi ensinado durante a explicação da GameSpy 2002. Sendo assim, vamos enfim aos testes de nossas implementações.

![#77](imagens/bungie_66.png)

#### Testando o projeto

Vamos aos testes. Para cada um deles, separei dois vídeos demonstrando o funcionamento do nosso projeto, com os servidores sendo exibidos novamente na server browser list, ou até uma gameplay.

**Testando o Halo CE**

[![KewGamespy - HALO CE DEMO](https://img.youtube.com/vi/2NNq4UT9Wgs/0.jpg)](https://www.youtube.com/watch?v=2NNq4UT9Wgs)

**Testando o Battlefield Vietnam**

[![KewGamespy - HALO CE DEMO](https://img.youtube.com/vi/EmE7mOZOakc/0.jpg)](https://www.youtube.com/watch?v=EmE7mOZOakc)

## Ideias Extras

##### Gameloader

Se você estiver perguntando como os módulos são injetados nos jogos da GameSpy para trabalho, ou como a etapa de desenvolvimento acontece, tudo isso ocorre através de um software que fica escondido na tray icon do Windows chamado de ```KewGameLoader``` ou, para os íntimos, ```KewGameSpyEmulatorClient```.

O KewGameLoader tem as seguintes capacidades:

- Identificar quando o processo dos jogos utilizando alguma SDK da GameSpy é iniciado e injetar os módulos que os corrigem.

- Estabelecer uma interface de debug funcional para que o desenvolvedor e contribuidor do projeto possam interagir e monitorar a performance e bugs do projeto.

- Exibir o status de jogo RPC no Discord usando a SDK de Desenvolvedor.

O ```KewGameLoader``` é um componente muito importante para tudo isso. Vamos explorá-lo de maneira técnica a partir de agora.

- **Identificando processos de jogos GameSpy via WMI:** o KewGameLoader, através da ```KewUsermodeProcessMonitor```, utiliza uma implementação de Query via WMI através de COM, de maneira a ser notificado sempre que um processo novo for criado pelo sistema operacional, causando um impacto mínimo no sistema operacional de nossos usuários. Caso você tenha alguma curiosidade de como isso funciona, consulte os arquivos ```KewUsermodeProcessMonitor.cc``` e ```KewUsermodeProcessMonitor.hh```.

- **Interface de debug:** através da ```GameIPC``` em uma thread segura, o KewGameLoader estabelece um PIPE Nomeado com o Módulo presente no jogo (Kurumi ou Kuromi), recebendo informações muito específicas de debugging e desempenho que são exibidas na tela do próprio jogo com um overlay. Caso tenha curiosidade, consulte o arquivo de cabeçalho ```GameIPC.hh```.

- **Interface de Overlay Segura:** através da ```DevOverlayFrame```, o KewGameLoader pode renderizar informações na tela do jogo utilizando OpenGL mais atualizado, sem que esse overlay ocorra internamente, podendo, em caso de algum problema, ser encerrado externamente. Caso tenha curiosidade, consulte os arquivos ```DevOverlayFrame.cc``` e ```DevOverlayFrame.hh```.

- **Discord RPC:** através do próprio arquivo principal ```KewGameLoader.hh```, adicionamos suporte à biblioteca ```Discord Developers SDK``` para que os jogos e status sejam exibidos na plataforma. Esse foi um recurso solicitado pelos testers do projeto.

![#78](imagens/kewgameloader1.png)

![#79](imagens/kewgameloader2.png)

![#80](imagens/kewgameloader3.png)

O ```KewGameLoader``` está em constante evolução, e pode ser que no futuro novos recursos sejam incluídos. Apesar dessa ser uma pesquisa inicial e base para permitir que os jogos nunca morram.

##### Dicas de modo Janela para Desenvolvimento

Se você estiver procurando uma maneira mais simplificada de trabalhar com a base fornecida aqui, com certeza não é em FullScreen. O maior problema dos Battlefields mais antigos é que, devido aos hooks de teclas e APIs desatualizadas da DirectX 8, encontramos diversos problemas e bugs de janela enquanto trabalhamos em melhorias no nosso projeto, principalmente se você estiver usando o Windows 11. E para resolver esse problema é bastante simples.

Separei em duas etapas, a primeira direcionada aos Battlefields e a segunda destinada ao Halo.

Para resolver o problema para Battlefield 1942 ou Vietnam:

- Vá até o binário do jogo no diretório de instalação.

- Vá nas propriedades do binário do jogo e em seguida no Modo de compatibilidade, selecione ```run this program in compatibility mode for``` e escolha ```Windows XP Service Pack 2```, depois salve.

- Agora, ainda nas propriedades, selecione a caixa ```Disable fullscreen optimizations```.

- Em seguida, vá para os seguintes diretórios: ```C:\Program Files\EA Games\Battlefield 1942\Mods\bf1942\Settings```, procure dois arquivos e abra-os em um editor de texto convencional: ```VideoDefault.con``` e ```Video.con```. Altere a linha ```renderer.setFullScreen``` para ```0```.

Para resolver o problema do Halo é muito mais simples:

- Vá para o atalho do jogo na área de trabalho e modifique a linha de comando do jogo para adicionar o argumento ```-window```.

##### TeaDelKew

Durante a reversão, desenvolvimento e escrita do artigo, surgiu a ideia de uma possível maneira para criptografar os payloads de forma similar utilizando um algoritmo parecido com o ```GOA``` (Gamespy Online Access), porém sem a compressão. Foi então que, após um bom tempo enfrentando o ```XTEA``` em outro projeto, tive a ideia de criar uma criptografia baseada nele, chamada por mim de ```TeaDelKew```. Não sou um especialista em matemática, porém introduzi algumas novidades no ```TeaDelKew```, sendo especificamente seis:

- Introdução do ```kew_box```, que são 12 valores constantes usados para a permutação dos dados aplicados antes do início dos ciclos de criptografia do XTEA.

- Diferente do XTEA convencional, o ```TeaDelKew``` utiliza 2048 iterações para cada uma das fases principais de criptografia e descriptografia, o que implica em uma maior segurança, mas ao custo de um pouco mais de uso computacional.

- O XTEA original usa uma aplicação simples da ```transformação de Feistel```; já o ```TeaDelKew``` combina, além da aplicação mencionada, uma manipulação XOR dos valores de iteração, bem como nega o resultado das operações delas.

- No ```TeaDelKew```, diferente do XTEA, logo no início, o texto raw ou já cifrado sofre uma negação antes do ciclo inicial de criptografia.

- As chaves do ```TeaDelKew``` são completamente diferentes das do ```XTEA```; cada parte sofre um XOR com a constante ```0xDEADBEEF``` antes de ser aplicada nos cálculos, modificando assim os valores originais.

- Por fim, a lógica de descriptografia do ```TeaDelKew``` espelha o algoritmo original de criptografia, aplicando um processo reverso e negação de resultados para obter os valores originais, mantendo assim algumas características encontradas no ```XTEA```.

Se estiver interessado na representação matemática disso (como aquelas encontradas em RFCs), admito que não sou bom nisso, mas dei o meu melhor para criar algo decente no meu tempo livre.

Para a ```tea_del_kew_encrypt```:

- v0 e v1 são as metades (DWORD) da entrada raw, inicialmente negadas.

- k0, k1, k2 e k3 são as chaves derivadas usando um XOR com ```0xDEADBEEF``` e um novo delta de ```0x00B0B0C4```.

- ```kew_box``` é uma tabela de substituição com 12 constantes hardcoded.

No processo de criptografia em si:

- Temos uma permutação inicial com ```kew_box``` e os valores das chaves, usando um laço de repetição de 0 até 2047:

![#81](imagens/kewtea1.png)

- Em seguida, aplicamos a transformação de Feistel modificada, usando um laço de repetição de 0 até 2047:

![#82](imagens/kewtea2.png)

Para a ```tea_del_kew_decrypt```:

- v0 e v1 são as metades (DWORD) já cifradas pelo ```TeaDelKew```.

- k0, k1, k2 e k3 são as chaves, obtidas da mesma forma que no processo de criptografia.

- sum é inicializado com a constante ```0x85862000```, e o delta usando a mesma constante da rotina de criptografia, neste caso ```0x00B0B0C4```.

No processo de descriptografia em si:

- Invertemos o processo de criptografia e primeiramente aplicamos a transformação de Feistel modificada inversa, usando um laço de repetição de 0 até 2047:

![#83](imagens/kewtea3.png)

- Por último, aplicamos a permutação inversa usando o kew_box e os valores das chaves, com um laço de repetição de 0 até 2047:

![#84](imagens/kewtea4.png)

Como pode perceber, tive o cuidado ao escrever o ```TeaDelKew```. Sinta-se à vontade para melhorar ou implementar completamente no projeto.

O código fonte está disponível no repositório em um projeto separado, apelidado de ```TeaDelKewTests```, e seu conteúdo inclui apenas um cabeçalho e um namespace chamado ```TeaDelKewAlgo.hh``` com a implementação adequada.

##### Error track com MiniDumps

Para uma maior estabilidade da solução proposta neste artigo, todos os módulos têm suporte para gerar MiniDump, o que permite identificar e resolver problemas. Isso pode ser expandido ainda mais no futuro. As principais informações sobre este recurso e onde encontrar os arquivos gerados são listadas abaixo:

- Para o módulo Kurumi, todos os MiniDumps são armazenados no diretório temporário em uma pasta chamada "KurumiBF1942", por exemplo: C:\Users\YOURUSER\AppData\Local\Temp\KurumiBF1942.

- Para o módulo Kuromi, todos os MiniDumps são armazenados no diretório temporário em uma pasta chamada "KuromiGS2004", por exemplo: C:\Users\YOURUSER\AppData\Local\Temp\KuromiGS2004.

Todos os dumps são gerados seguindo o padrão da Microsoft (momento do crash + prefixo .dmp).

## Bla, Bla Juridico

Ao escrever este artigo, consultei um advogado. Desta forma, fui aconselhado a adquirir todas as licenças possíveis para todos os jogos mencionados. Sendo assim, os seguintes títulos foram adquiridos:

- Battlefield 1942, versão digital, adquirido de um colecionador registrado originalmente na plataforma Origin em 2010 (ano de compra).

- Battlefield Vietnam, versão física, adquirido de um colecionador que comprou na loja GameStop por volta de 2006.

- Halo CE, versão disponibilizada pela própria Bungie.

Outros detalhes importantes:

- Fiz várias tentativas com o suporte técnico da EA Games para adquirir a versão digital de Battlefield Vietnam. Ambos insistiram que o jogo não era mais suportado e que não se responsabilizavam por ele. Um ponto positivo foi a tentativa deles de ativar a versão digital na minha conta, além da excelente atenção recebida. Houve até felicitações pelo cuidado com essas cópias clássicas e surpresa de como consegui uma cópia lacrada desse jogo.

Portanto, tudo o que foi necessário para a construção deste artigo, desde as licenças de software até a autenticidade dos jogos originais, foi providenciado. Saliento que em nenhum momento será disponibilizada uma versão jogável dos jogos, e partes de arquivos de jogos, assim como links para websites de terceiros que contenham downloads correspondentes, não configuram violação da Lei de Direitos Autorais do Brasil, Seção 107 de 1976. Além disso, arquivos de patch ou melhorias também não infringem direitos autorais, o que se aplica igualmente a todo o código fonte e material gerado com esta pesquisa.

## Revisores, Testers e Agradecimentos especiais

- Anderson Leite(github.com/buzzer-re): Por ter revisado meu artigo e colaborado com sugestões e aprimoramentos.

- Akko(github.com/AkkoS2): Por ter testado as funcionalidades em gameplay.

## Conclusão

Nossa conclusão é bem aberta; este artigo certamente proporcionou momentos gratificantes de aprendizado e satisfação pessoal ao destacar títulos que foram importantes para mim de diversas formas, além de assegurar que sejam apreciados por futuras gerações. A oportunidade de reavivar o sentimento nostálgico em comunidades ou pessoas interessadas é muito gratificante. Espero que este artigo possa ser utilizado para gerar ainda mais melhorias ou facilitar futuras iniciativas nesse sentido.

[![SER GAMER 2... Zangado](https://img.youtube.com/vi/18DLBcMg_5k/0.jpg)](https://www.youtube.com/watch?v=18DLBcMg_5k)

! Finalizo com um vídeo de referência que provavelmente apenas brasileiros vão conseguir compreender.

Keowu \^-^

## References

LUIGI. Auriemma Luigi Enctype 2 Papers. [S. l.], 27 abr. 2007. Disponível em: https://aluigi.altervista.org/papers.htm. Acesso em: 30 mar. 2024.

ERRI120. Breaking EA Desktop‘s pathetic Encryption. [S. l.], 18 jan. 2023. Disponível em: https://erri120.github.io/posts/2023-01-18/. Acesso em: 31 mar. 2024.