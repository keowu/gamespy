# Revivendo velhos tempos - Engenharia Reversa e Reimplementação do multiplayer de meus jogos antigos

Author: João Vitor (@Keowu) - Security Researcher

## Introdução

Este artigo visa documentar e também apresentar meus passos durante os estágios de engenharia reversa de três grandes clássicos da minha infância e adolecência. sendo eles Battlefield 1942, Vietnam e Halo CE a ideia por trás desse artigo se deu após eu encontrar meu antigo computador no sotão da minha casa. e ao liga-lo visualizar os icones em meu desktop destes grandes classícos. ao final deste artigo você vai ser capaz de entender como funcionam as implementações, protocolos de comunicação desses jogos, vamos reescrever totalmente todas as listas de servidores desses jogos permitindo que voltem de volta a vida como eram em sua época de glória, explicando cada detalhe e fornecendo todo o código fonte para posteriores pesquisas, usos. ou até mesmo regatar o sentimento de nostalgia dos clássicos. além disso você também sera capaz de reverter e reviver todos os classicos que tem a GameSpy SDK embutida neles.

### Uma leve motivação

No clima nostalgico que esta pesquisa nos apresenta. outro grande clássico dos mundos dos games(em especial da minha terra natal. O Brasil), do meu youtuber favorito dos games Zangado e suas maravilhosas palavras de reflexões dos games e da vida.

[![SER GAMER... Zangado](https://img.youtube.com/vi/j-wH8EPJ03U/0.jpg)](https://www.youtube.com/watch?v=j-wH8EPJ03U)

PS: Não espero que um não nativo brasileiro entenda o peso desta referência(apenas respeite o momento), sendo assim siga com o decorrer do artigo.

Essa pesquisa tem uma certa importância pessoal, enquanto escrevia lembrei-me de ótimos momentos com meus amigos do Ensino Fundamental e Médio, do quanto me diverti jogando esses clássicos, da quantidade de pessoas que conheci(brasileiros e estrangeiros) e de como a simplicidade nos faz Tão Feliz. nesta época com certeza eu não tinha acesso aos melhores hardwares do mercado mas o pouco que tinha, me moldaram a lutar e chegar nos meus objetivos. espero sinceramente que você não leia isso com o olhar de um idiota e superioridade, que afogue a sua própria idiotização e transforme suas ideias em uma incansável luta pela própria evolução!

### GameSpy da glória a Decadência

Se você jogou algum jogo clássico de franquias como Battlefield, Halo, Arma, Crysis, Star Wars. até Jogos multiplayer do Playstation 2(sim isso era possível) e Nitendo Wii. com toda certeza você usou algum serviço da GameSpy:

![#1](imagens/gslogo.png)

Gamespy foi criada pelo Engenheiro de Software Mark Surfas, e provia apenas serviços de listagem de browser para o Quake(1996) e também foi nessa época que começaram a comercializar o serviço para empresas desenvolvedores de jogos(como EA e seu Battlefield 1942, em 2002). porem foi em 2004 que o número de títulos que utilizavam o serviço teve um crescimento exponencial com a aquisição pela IGN.

A GameSpy mostrava-se viavel para desenvolvedores, em uma época que manter um serviço de multijogadores estável e funcional demandaria muito tempo de desenvolvimento. a SDK provia aos desenvolvedores tudo que era preciso, desde server browser, até autenticação(como no caso de alguns jogos como Battlefield 2, Battlefield 2142 e jogos do playstaion 2). SDK multiplataforma e facilmente portavel para diversas plataformas fornecendo apenas uma callback para os desenvolvedores obterem os dados necessarios focando apenas na criação e design do server browser dos jogos. até mesmo as interfaces e aplicações server-sides auto-hospedadas pelos jogadores utilizavam essa mesma base de SDK provida pela GameSpy. 

![#2](imagens/gsad.jpg)

Infelizmente, manter um serviço desse tamanho era extremamente caro, e pouco rentável, foi então que em Abril de 2014 a IGN decide encerrar os seus serviços, matando completamente diversos títulos clássicos de diversas franquias clássicas. na época a IGN ofereceu o código fonte completo e backend as empresas que usavam seus serviços para que pudessem hospedar por conta e lançarem patchs corrigindo os jogos. algumas empresas como a EA Games, apenas ignoraram esse fator e deixaram suas franquias clássicas morrerem. outras como o caso da Bungie hospedaram o backend recebido da IGN em algum lugar, por exemplo a Bungie mantem uma instância EC2 da Amazon rodando o serviço de server browsing para seus clássicos como o Halo Combat Evolved, além é claro de lançarem um patch aos seus jogos corrigindo este problema. porem isso são excessões.

## Battlefield 1942

Battlefield 1942 foi o primeiro clássico da franquia Battlefield lançado pela EA Games em 2002. ele contava com uma implementação de uma das primeiras SDKs desenvolvidas pela Gamespy. esta versão já continha a criptografia proprietária de pacotes apelidada de GOA(Gamespy Online Access) este algoritmo combinava chaves randômicas simetricas e compressão de dados além de rounds shufle com XOR. em volta do payload recebido do MasterServer. esses dados eram então parseados, verificados, obtidos mais informações sobre os servidores e então retornado em uma função callback fornecida pela própria EA Games ao inicializar a struct de configuração do GameSpySdk(rotina essa apelidada de "SBListCallBackFn" com base em informações obtidas com informações em metadados de binários dessa época).

#### Analisando binário

O binário original do jogo Battlefield 1942(versão de CD) não continha nenhum DRM como os atuais em seu binário, é claro que possuia alguma proteção anti-pirataria como é o caso do **SafeDisc** que utilizava o bom e velho vulneravel **"secdrv.sys"** que apenas impedia sua execução sem o disco original. facilmente ignorado com o advento da pirataria no Windows XP. atualmente é bem difícil encontrar essas versões já que apenas edições de colecionadores estão disponíveis no mercado. além disso no Windows 10 é inútil tentar carregar esse driver já que em 2015 a Microsoft simplesmente bloqueou seu carregamento devido a compatibilidade e segurança, já que existiam exploits para ele. Dado isso os desenvolvedores foram avisados bem antes sobre este problema, já que em 2012 a própria EA Games removeu a SDK do binário do jogo. adicionando então o Origin-DRM. Origin DRM(Agora EA-Launch DRM) utiliza AES para criptografar os dados de seções do binário PE de seus jogos, e as chaves são armazenadas conforme a licença do usuário em um arquivo apelidado de "BF1942.par". o processo de loading através desse DRM descriptografa todos os dados através de uma nova seção adicionada no binário PE sem nome:

![#3](imagens/encsection.png)

Essa nova stub feita manualmente em assembly que carrega uma DLL apelidada de **"Activation.dll"** e chama seu export também sem nome utilizando apenas o **ordinal**. a partir disso em memória a DLL reconstroi toda Tabela de Importação e descriptografa o conteúdo da seção ".text" em blocos de 16 bytes e por fim chama o Entrypoint original, veja a nova stub adicionada nos jogos reponsável por fazer esse procedimento:

![#4](imagens/ealoader.png)

O código do jogo é o mesmo das versões pirateadas encontradas na internet **v1.61**, com a pequena diferença de não possuir não tem nenhuma alteração além de remover o DRM-SafeDisc.

Uma curiosidade é que os desenvolvedores da DICE usaram o **VirtualXP** e uma nova branch do SVN para gerar as buils do Jogo com a implementação do novo DRM:

![#5](imagens/compilemetadata.png)

Não entrarei em maiores detalhes de como esse DRM funciona, até porque existem conteúdos explorando outras partes do mesmo. além é claro de não querer problemas judiciais(dado que isso é uma pesquisa para reviver o jogo e não piratea-lo). porem deixo a curiosidade da criatividade do time de segurança da EA Games ao proteger seus jogos.

![#6](imagens/ea_money_shit.gif)

Para continuar com a versão da Origin(Ea Launcher) você pode simplesmente deixar o loader fazer o trabalho pesado. reconstruir a IAT e apenas definir o EntryPoint(coisa que não sera demonstrado aqui), porem apenas saiba que tudo que for demonstrado aqui é aplicavel na versão da Origin dado que a base de código é a exatamente a mesma, só que sem o DRM antigo.

Hoje em dia se você apenas obter uma cópia do Battefield 1942 de algum lugar, como Origin, Disco ou de algum lugar nos confins da internet. e for jogar online simplesmente não vai ser possível, já que a lista de servidor não existe mais. porem se você encontrar algum endereço IP de servidor na internet, ou você mesmo hospedar um servidor é possível jogar adicionando-o nos favoritos.

**Você pode se perguntar o porque isso ocorre ?**

Basicamente quando você já conhece um endereço IP não há necessidade da masterserver list. já que a própria SDK da Gamespy envia requisições solicitando detalhes do servidor presente na lista e como ele esta ativo apenas retorna as devidas informações para você.

**Agora você deve estar se perguntando, como isso ocorre ??**

Vamos descobrir a partir de agora analisando e revertendo completamente o Battlefield 1942 com a implementação da SDK da Gamespy 2002 presente nele.

![#7](imagens/genericanimestuff2.gif)

#### Revertendo pacotes

Algumas pessoas tiveram acesso a SDK completa do Gamespy no momento de fechamento dos servidores da Gamespy principalmente grandes servidores desses jogos que entraram em contato para obter, não sendo o meu caso. durante a demonstração de como funciona a comunicação entre cliente e masterserver. e cliente e servidor, usarei minha própria reimplementação da lógica usada pelo Masterserver da Gamespy revertido. e logo mostrarei a implementação no binário analisado do jogo.

O Battlefield 1942 utiliza as seguintes portas:

1. **28900** - Para comunicação ao Masterserver list provider através do protocolo TCP.
2. **23000** - Para comunicação e obter informações dos servidores através do protocolo UDP.
3. Demais conexões utilizam o protocolo TCP e não serão abordadas.

Quando uma nova solicitação de atualização da server browser ocorre, através do clique do botão abaixo:

![#8](imagens/bf1942_1.png)

Uma conexão TCP com o endereço da Masterserver através da porta 28900 é efetuado. e quando a conexão é aceita a primeira requisição é recebida pelo cliente:

![#9](imagens/bf1942_2.png)

O comando recebido pelo cliente é o seguinte:

```\\basic\\secure\\MASTER```

Este comando indica que um backend da Gamespy para server browser provider esta disponível para ser consultado. isso trata-se de uma apresentação do Masterserver ao cliente conectado.

Em seguida o Masterserver espera que o Cliente forneça uma query para o jogo em questão. essa query é composta pelas seguintes informações:

1. Gamename.
2. Versão do Jogo.
3. Location, sempre por padrão zero(isso é diferente no caso de implementações do Playstation 2, que possuiam jogos diferentes por região).
4. Validação, Uma chave hardcoded padrão. única para cada jogo, podendo ser considerado uma chave de API.
5. Enctype, padrão 2, porem isso se referia a versão do algoritmo do GOA(Gamespy Online Access)
6. O magic delimitador para indicar o final dos frames, nesse caso sempre ascii "final", isso vai fazer mais sentido ao decorrer da explicação.
7. Query ID com o padrão sempre sendo 1.1

![#10](imagens/bf1942_3.png)

Seguido disso temos a requisição última requisição a qual com base na query acima solicita que o Gameserver Provider envie o buffer comprimido e protegido com o algoritmo GOA(Gamespy Online Access):

![#11](imagens/bf1942_4.png)

Diferente do **query**. o list não tem nenhum significado ou configuração a ser explicado ele apenas fara o request do buffer contendo o payload com os servidores a serem usados.

Por último após todo esse processo o buffer protegido é enviado pelo masterserver provider totalmente criptografado(não sendo o meu caso, porque eu removi o algoritmo já que minha solução simplesmente não precisa dela):

![#12](imagens/bf1942_5.png)

Antes de explicar como isso foi implementado e mostrar toda a explicação da engenharia reversa por trás. esta é uma masterserver provider real hospedada no brasil escrita e revertida no meu tempo livre, que agora vai se tornar OpenSource. 

![#13](imagens/genericanimestuff1.gif)

Vamos olhar como as requisições ocorrem diretamente na implementação da SDK da Gamespy presente no Battlefield 1942. 

#### Revertendo o código fonte e implementação da Gamespy

Vamos iniciar pelo procedimento apelidado por mim de ```decrypt_tcp_trafic```, Uma curiosidade aqui é que a SDK da Gamespy era completamente desenvolvida em C. e deveria ser feito mesmo, até porque isso facilitava muito ao portar para outras plataformas/arquiteturas. bom, continuando. voltando a explicação do procedimento, ele é responsável por definir configurações ao socket, e ler o buffer que estiver pendente nele, descomprimir, e descriptografar, tudo isso com o tamanho máximo de 2047(sendo esse, o tamanho máximo que um buffer da Gamespy pode suportar), além é claro de parsear os dados e adicionar em uma callback(da própria Gamespy) para que sejam validados e por fim adicionados a uma lista ligada e passados a callback fornecida por quem implementava a SDK, nesse caso, a DICE. outras callbacks também eram implementadas como para tratativas de erros, tudo fara mais sentido conforme eu apresento.

Começando do início temos uma chamada para ```select``` e seguido de uma chamada para ```recv```. o objetivo aqui é monitoras eventos para leitura do socket  definindo um timeout de 0 segundos para cada evento de leitura do socket TCP estabelecido. retornando um código de erro ao GameSpy Switch(um bloco de código que intereja em loop responsável por responder a eventos externos direcionados ao SDK além de eventos do socket como de apresentação e autenticação no Masterserver provider) o código retornado em questão é **STATUS_TIMEOUT_ERROR**(obviamente isso foi definido por mim com base na analise do comportamento do código e minha interpretação dele). logo após ocorre de fato a leitura do buffer com tamanho máximo de ```2047``` esse valor constante é o tamanho máximo que um payload(conjunto criptografado ou descriptografado com os frames de servidor para consulta) pode assumir aqui perceba também que uma global é utilizada ```readed_buffer``` uma DWORD que em questão armazena o valor já lido com o tamanho do payload anteriormente caso uma nova intereção seja necessária. isso ocorre por exemplo se por algum motivo o payload estiver corrompido.

![#14](imagens/bf1942_6.png)

Todos os dados lidos do payload são armazenados em uma global. em um ponteiro composto por duas globais. e aqui mais uma vez chama a criatividade da implementação. a primeira global ```FirstFrameByte``` é um byte. seguido de ```readed_buffer``` que carrega o offset/tamanho do buffer anterior, cabe-se destacar que na maioria das vezes esse valor se assume ```0``` por padrão. 

Você deve estar se perguntado agora:

**Como assim um único byte não vai transbordar ?**

![#15](imagens/genericanimestuff3.gif)

A resposta é, sim! e esse é o objetivo mesmo. o desenvolvedor alinhou muito bem os dados. ao ler o buffer sempre vai ser com um tamanho superior a um byte dessa forma sobrescrevendo outra variavel global apelidada de ```frame_buffer``` ela que de fato vai carregar o buffer sem o magic byte(que por coincidencia é sempre um único byte que com um XOR **0xEC** resulte em algo superior a 0, quando zero indica que a geração do payload foi negada), veja um exemplo:

![#16](imagens/bf1942_7.png)

Agora com os devidos dados, hehe:

![#17](imagens/bf1942_8.png)

Seguido disso temos as verificações do tamanho do buffer lido:

![#18](imagens/bf1942_9.png)

Seperei este trecho para mostrar coisas curiosas para você leitor. aqui podemos ver as validações feitas pela Gamespy em relação a tratativa de erros quando um buffer não é obtido do socket TCP. mas perceba que como mencionado por mim anteriormente. uma callback registrada por quem implmentou a SDK em determinado jogo é registrado. **clsSocket->GamespyCallback** nada mais é que um endereço de um procedimento da própria DICE que recebera o código de erro forneceido pela Gamespy e com base em um switch case tratara exibindo a devida mensagem de erro para ele. uma segunda curiosidade aqui é que os parâmetros na declaração podem assumir qualquer coisa por quem implementar já que é um **PVOID**. nesta mesma callback as informações de lista ligada de servidores são retornados.

Bom, indo mais a fundo vamos entender como os dados de um raw buffer, que é um payload, vão ser tratados até obtermos informações relevantes que nos permitam entender o que trafega nesse procedimento.

![#19](imagens/bf1942_10.png)

A partir de agora vamos ver a utilização do GOA(Gamespy Online Access) em dois procedimentos, o primeiro ```decompress_one``` e o segundo em ```decompress_two```, além do primeiro estágio de parsing dos dados em um ponteiro para uma struct de dados revertida por mim, a ```server_data_struct```.

A primeira verificação em ```clsSocket->already_decrypted != -1``` é usada nos casos já mencionados acima, onde a descriptografia dos payload já foi feita anteriormente, porem ele estava corrompido de alguma forma, então não há necessidade de trabalhar com os cabeçalho novamente, apenas com os dados do body do payload. e o algoritmo responsável por isso estão no procedimento ```decompress_two```. Avançando, caso seja a primeira interação com os dados do payload veremos todo o processo de descompressão e inicialização da chave e decompressing do cabeçalho de dados, o marco para inicio dessa etapa é a comparação em ```already_decrypted == -1```:

![#20](imagens/bf1942_11.png)

Logo no início desta etapa a primeira verificação usando o byte magic 0xEC ocorre, seguido também da inicialização da chave que esta no cabeçalho do payload através da operação de XOR ```frame_buffer[i] ^= clsSocket->ucRandomicKeyBytes[i + 40]``` a cada inicialização as chaves em ```ucRandomicKeyBytes``` mudam e são construídas com base em um rand, mas esse é só o primeiro layer de criptografia em cima dos dados que ainda contam com a compressão. após a inicialização da chave o procedimento ```decompress_one``` é usado para descriptografar o cabeçalho do payload. logo após isso já temos o valor do offset para o body do payload que é determinado pelo XOR do byte magic com 0xEC ```FirstFrameByte ^= 0xECu``` onde era utilizado para acessar ```frame_buffer``` interpretado como uma referência ```PVOID``` que posteriormente identifiquei se tratar de uma struct(pois era acessada via indices no assembly, similar a padrão adotados por structs), dessa forma criei a seguinte struct:

```c++
struct server_data_struct {

  DWORD server_ip;
  WORD server_port;

};
```

Onde cheguei no seguinte resultado, facilitando melhor nossa visualização do que ocorria:

```c++
gameservers = (server_data_struct *)&frame_buffer[FirstFrameByte]
```

Onde por fim era utilizada como argumento para que o conteúdo do body do payload fosse descriptografado com uma chamada para "decompress_two", no seguinte trecho:

```c++
decompress_two(clsSocket->ucBuffer, gameservers, (PVOID)(&FirstFrameByte + readed_buffer - (UINTPTR)gameservers));
```

Essa era toda a rotina de criptografia e trabalho com o frame adotado pela Gamespy nessa versão. apesar de ter feito um porte completo dessa rotina de descriptografia mais a diante em IA-32. eu não dediquei tempo em analisa-la a fundo. no entanto ela era a mesma compartilhada por diversos jogos que usavam essa versão da SDK, caso você se interesse em entender recomendo ler o paper de um outro pesquisador independente, ```LUIGI. Auriemma``` em específcio em um paper apelidado de ```GS enctype2``` disponível nas referências do artigo. mais adiante introduzi uma nova criptografia nesses frames utilizando uma implementação modificada e pessoal do XTEA, apelidada de ```TeaDelKew```.

Logo após todo esse processo em relação ao algoritmo de descriptografia chegamos a etapa de criação da lista ligada responsável por criar uma fila de processamento para verificar o estado do servidor utilizando o protocolo ```UDP``` com a porta padrão ```23000```.

![#21](imagens/bf1942_12.png)

Nesse procedimento cada membro da struct é acessado, o IPV4, a PORTA. e o ponteiro da struct é incrementado em 6 bytes. o tamanho exato da própria struct para acessar o próximo conjunto de dados. logo em seguida todos esses dados são passados como argumento para a lista ligada. em ```add_to_list_to_check_status_via_UDP```. 

Se você estiver se perguntando como é definido o tamanho máximo do payload nesse parsing. ele ocorre com base na assinatura(UTF-8) ```\\final\\```:

![#22](imagens/bf1942_13.png)

Nesse procedimento quando o último frame for parseado a endFlag assumira o valor padrão 7. e então o procedimento é encerrado. tudo isso enquanto não for encontrado a assinatura ```\\final\\``` que delimita exatamente o tamanho máximo caso algo falhe.

Vamos agora dirigir nossa atenção em como a validação dos servidores é feita pela Gamespy SDK usando o protocolo UDP. essa verificação pode ser considerada uma especíe de heart-beat, já que a comunicação ocorre apenas para obter detalhes do servidor como números de jogadores, mapas e nome do mapa jogado. vamos iniciar analisando os pacotes:

![#23](imagens/bf1942_14.png)

Quando a Gamespy SDK vai verificar um servidor, o primeiro comando enviado é o ```\status\```, este comando é usado para solicitar as devidas informações de heart-beat do servidor em questão. o servidor então responde com outros payloads, como do exemplo abaixo:

```
\gamename\bfield1942\gamever\v1.61\language\English\location\1033\averageFPS\0\content_check\0\dedicated\1\gameId\bf1942\gamemode\openplaying\gametype\ctf\hostname\NOME DO SERVIDOR | LOL NOME DO SERVIDOR\hostport\14567\mapId\BF1942\mapname\wake\maxplayers\64\numplayers\36\password\0\reservedslots\0\roundTime\1200\roundTimeRemain\274\status\3\sv_punkbuster\0\tickets1\0\tickets2\4\unpure_mods\\version\v1.61\active_mods\bf1942\allied_team_ratio\1\allow_nose_cam\yes\auto_balance_teams\on\axis_team_ratio\1\bandwidth_choke_limit\0\content_check\0\cpu\3766\external_view\on\free_camera\on\game_start_delay\15s\hit_indicator\on\kickback\10%\kickback_on_splash\10%\name_tag_distance\200\name_tag_distance_scope\350\number_of_rounds\1\soldier_friendly_fire\40%\soldier_friendly_fire_on_splash\40%\spawn_delay\4s\spawn_wave_time\7s\sv_punkbuster\0\ticket_ratio\250%\time_limit\30\tk_mode\forgive\unpure_mods\\vehicle_friendly_fire\40%\vehicle_friendly_fire_on_splash\0%\queryid\6903.1
```
![#24](imagens/genericanimestuff4.gif)

**Ficou confuso em relação as informações retornadas ?**

Primeiramente, na época de lançamento "json" nem era tão comum assim, na realidade ele estava começando a engatinhar(já que ele surgiu nesse mesmo ano). mas isso não é um problema, tirando que as versões de 2005/2006 que utilizavam o XML nas comunicações da Gamespy, porem isso era padrão mesmo para o sistema de registro de jogadores, já este modelo apresentado era bem comum e funcionava muito bem para o padrão necessário. esse padrão não era exclusivo para a DICE/EA ou seja esse mesmo modelo era compartilhado entre os jogos da época. diferente aqui são apenas as informações que eram recebidas, já que cada jogo parseava de maneira diferente. alguns padrões se mantinham, destacarei eles agora:

- **gamename** - Representa o nome do jogo que o servidor era dirigido nesse caso "bfield1942"
- **gamever** - Representa a versão do jogo, nesse caso é v1.61(a versão mais atual do jogo) e caso essa versão fosse diferente do cliente uma callback de erro é ativada
- **hostname** - Representa o nome do servidor em questão
- **hostport** - Representa a porta do servidor
- **queryid** - Representa o id da query feita naquele servidor, nesse caso 6903.1

Esses eram apenas a parte padrão do frame(que outros jogos também possuem) mas a Gamespy era altamente adaptavel e todos os outros fields eram direcionados ao Battelfield 1942.

A primeira parte apresentada era apenas a introdução recebida do servidor. outras informações padrões também eram recebidas, como por exemplo informações dos jogadores atuais daquele servidor, K/D(Kill, Death), ping e time jogado. além é claro da keyhash(algo bem curioso que vou apresentar em breve para o que ela serve).

```
\deaths_0\2\keyhash_0\e09833b4065c325fbe042d4c7cf8bba9\kills_0\0\ping_0\41\playername_0\[Admin]Cher\score_0\0\team_0\1\deaths_1\10\keyhash_1\d8e803a4c432c5dbc129805bb13c5ae9\kills_1\0\ping_1\52\playername_1\Jim Lahey\score_1\13\team_1\1\deaths_2\0\keyhash_2\92ff73033c3351d61131b3129990aa77\kills_2\0\ping_2\17\playername_2\jake from state farm\score_2\0\team_2\1\deaths_3\7\keyhash_3\114d0844ea3df43d48840016f2b4a7ae\kills_3\17\ping_3\24\playername_3\TheCalmingClam\score_3\34\team_3\2\deaths_4\2\keyhash_4\93b6ad5f0452ee5ff76702ae3f071e9e\kills_4\19\ping_4\21\playername_4\mediocre\score_4\23\team_4\2\deaths_5\4\keyhash_5\860bd844411700821c06d91751a22f13\kills_5\10\ping_5\25\playername_5\*Incognito*\score_5\26\team_5\2\deaths_6\12\keyhash_6\e189e41fc7254f4f1ad351b0aadeeeeb\kills_6\4\ping_6\151\playername_6\GLIER\score_6\13\team_6\2\deaths_7\6\keyhash_7\8f2cbe5d55b6f974199a0136b91791c4\kills_7\7\ping_7\31\playername_7\Lagtastic\score_7\26\team_7\1\deaths_8\7\keyhash_8\d0db7b12341ec39e07dbf686d7c0663e\kills_8\1\ping_8\77\playername_8\papi\score_8\12\team_8\2\deaths_9\9\keyhash_9\57592dcf0786e10a376577ad33c9d8d6\kills_9\7\ping_9\59\playername_9\Meuse\score_9\13\team_9\1\deaths_10\7\keyhash_10\8e024e9945e8cef5565e7eb67c63356b\kills_10\7\ping_10\44\playername_10\Sorrowful Rice\score_10\40\team_10\2\deaths_11\17\queryid\6903.2
```

Como mencionado aqui todos os fields eram customizados para o Battlefield 1942, diferentes de outros jogos. é claro que ainda tinha algum padrão como é o caso da própria ```queryid```. A ```keyhash``` era exatamente a hash do seu serial de ativação armazenado em algum lugar do sistema operacional(é óbivio que não entrarei em detalhes de onde, porque tenho medo da EA Games e da DICE).

Uma curiosidade do jogo é que a verificação de serial era feita ao lado do cliente, e isso funcionava da seguinte forma: Imagine que você deseja entrar em um servidor mas o seu serial é o mesmo de um jogador já conectado, isso era validado através do field ```keyhash```. e caso o seu fosse o mesmo de um jogador já conectado, você receberia um erro(uma callback de verificação da Gamespy, a mesma já detalhada anteriormente).

**Um problema aqui é que se você copiar alguma dessas keyhash e substituir pela sua você consegue se passar por outro jogador.**

![#24](imagens/genericanimestuff5.gif)

##### [Bonus] Explorando segurança das chaves de registro para abusar do problema de serial dos sevidores para desconectar jogadores

Se você também estiver curioso de onde essas informaçoes de serial estão armazenadas, nada melhor do que crackear um keygen/changer, obviamente sem entrar no critério de pirataria. eu reverti um antigo changer de serial do Battlefield 1942, esse gerador usava um ASPack e era completamente feito em Delphi:

![#25](imagens/bf1942_15.png)

![#26](imagens/bf1942_16.png)

Este software se tratava apenas de um changer. seu objetivo era apenas alterar as chaves de ativação do jogo. ele não era um keygen. sua única validação era se o serial cumpria com o tamanho de 22 digitos. e apenas o gravava em duas chaves de registro sem nenhuma segurança, como demonstrado abaixo:

```\\SOFTWARE\\Electronic Arts\\EA GAMES\\Battlefield 1942\\ergc\\```

![#27](imagens/bf1942_17.png)

```\\SOFTWARE\\Aps\\cdkey\\```

![#28](imagens/bf1942_18.png)

###### Como era gerado a hash para a serial key ? a mesma usada nos servidores para verificação ?

Para responder essa pergunta, podemos olhar a própria implementação do Battlefield 1942:

![#29](imagens/bf1942_19.png)

O valor armazenado na chave de registro é passado utilizado para gerar uma hash MD5 dos valores. é possível confirmar que se tratava de um MD5 apenas observando as constantes de inicialização presentes no procedimento "generate_data_md5":

![#30](imagens/bf1942_20.png)

Além do output bater com alguns softwares de keygen disponíveis na internet:

![#31](imagens/bf1942_21.png)

Se você estiver se perguntando o que aconteceria se dois seriais com a mesma hash estivessem em um mesmo servidor, ambos levariam disconnect, e podemos abusar disso apenas hookando um procedimento de implementação do md5, sera que podemos considerar uma vulnerabilidade ? fique a sua própria interpretação pessoal.

![#32](imagens/genericanimestuff6.gif)

##### Analisando a implementação de verificação de servidores da Gamespy SDK no Battlefield 1942

Após vermos como o serial funcionava e suas vulnerabilidades de segurança, Agora vamos retornar nossa atenção para ver como a verificação de servidores era implementado pela Gamespy SDK, diretamente no fonte da implementação do Battlefield 1942:



#### Escrevendo um parser para pacotes

#### Analisando e criando ideias para modificações

#### Analisando padrões de código

#### Escrevendo uma nova masterserver provider

## Halo Combat Evolved

## Battlefield Vietnam

## Futuras ideias

## Bla, Bla Juridico

Ao escrever este artigo um advogado foi consultado. desta forma fui aconselhado a adquirir toda e qualquer licença possível de todos os jogos possíveis. sendo assim. os seguintes títulos foram adquiridos:

- Battlefield 1942 versão digital, adquirido de um colecionador registrado originalmente na plataforma Origin em 2010(mesmo ano de compra). 
- Battlefield Vietnam versão física, adquirido de um colecionador no qual comprou em uma loja GameStop em meados de 2006.
- Halo CE versão disponiblizada pela própria bungie.

Outros detalhes necessários:

- Efetuei diversas propostas ao suporte técnico da EA Games para adquirir a versão digital de Battlefield Vietnam. ambos insistiram que não era um jogo mais suportado e que não ligavam para o que ocorresse com ele. Um ponto positivo do suporte técnico da EA foi a tentativa por parte deles de ativarem a versão digital na minha conta e uma excelente atenção, até parabenizando pelo cuidado com as cópias desses clássicos.

Sendo assim tudo que foi necessario para construção deste artigo. desde a licença de softwares. como os próprios jogos são originais. e saliento que em nenhum momento uma versão jogavel sera disponibilizada(partes de arquivos de jogos bem como links para Websites de terceiros que contém devidos downloads não se enquadram em uma violação da lei da Seção 107 da Lei de Direitos Autorais de 1976 do Brasil), saliento que arquivos de patch ou melhoria também não se enquadram como uma violação de direito autoral, dessa forma o mesmo se aplica a todo código fonte e material gerado com este material.

## References

LUIGI. Auriemma Luigi Enctype 2 Papers. [S. l.], 27 abr. 2007. Disponível em: https://aluigi.altervista.org/papers.htm. Acesso em: 30 mar. 2024.

ERRI120. Breaking EA Desktop‘s pathetic Encryption. [S. l.], 18 jan. 2023. Disponível em: https://erri120.github.io/posts/2023-01-18/. Acesso em: 31 mar. 2024.