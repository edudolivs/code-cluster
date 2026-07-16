# Sistema de Serviço de LLM

**Nome:** Eduardo de Oliveira Silva
**Matrícula:** 20250018691

## Descrição do Domínio

Este projeto modela um sistema de serviço de Large Language Models (LLMs).
O sistema modela um assistente de IA baseado em LLM. Um `AiAssistant` coordena
pedidos do usuário utilizando um `Model` de linguagem e um conjunto de `Tool`s.
Cada sessão de uso (`Session`) pertence a um `User` e armazena um histórico de
`Message`s trocadas. As relações incluem composição (mensagens dentro de sessões,
ferramentas dentro do assistente) e agregação (modelo e usuário referenciados).

## Diagrama UML de Classes

```mermaid
classDiagram
    class User {
        -string name_
        -string email_
        +get_name() string
        +get_email() string
        +display_info() string
    }

    class Model {
        -string name_
        -int max_tokens_
        -double cost_per_token_
        +get_name() string
        +get_max_tokens() int
        +get_cost_per_token() double
        +estimate_cost(int) double
    }

    class Tool {
        <<abstract>>
        -string name_
        -string description_
        -bool enabled_
        +execute(string)* string
        +cost_per_call()* double
        +describe() string
        +get_name() string
        +get_description() string
        +is_enabled() bool
        +toggle() void
        +~Tool() virtual
    }

    class WebSearchTool {
        -int calls_
        +execute(string) string
        +cost_per_call() double
        +describe() string
        +get_calls() int
    }

    class CalculatorTool {
        <<final>>
        +execute(string) string
        +cost_per_call() double
    }

    class Message {
        -string role_
        -string content_
        -string timestamp_
        +get_role() string
        +get_content() string
        +get_timestamp() string
        +format() string
    }

    class Session {
        -int id_
        -vector~Message~ messages_
        -User* user_
        +get_id() int
        +get_user() User*
        +get_message_count() size_t
        +add_message(Message) void
        +summarize() string
        +~Session()
    }

    class AiAssistant {
        -string name_
        -Model* model_
        -vector~Tool*~ tools_
        -vector~Session*~ sessions_
        +get_name() string
        +set_model(Model*) void
        +add_tool(Tool*) void
        +create_session(User*) Session*
        +list_tools() string
        +status_report() string
        +~AiAssistant()
    }

    AiAssistant "1" o-- "1" Model : utiliza
    AiAssistant "1" o-- "0..*" Tool : disponibiliza
    AiAssistant "1" *-- "0..*" Session : gerencia
    Session "1" *-- "0..*" Message : contém
    Session "1" o-- "1" User : pertence a
    Tool <|-- WebSearchTool : herda
    Tool <|-- CalculatorTool : herda
```

## Composição e Agregação (Questão 3)

### Relações de Composição (◆)

- **AiAssistant ◆── Session**: composição — o `AiAssistant` cria as sessões internamente
  (`create_session`) e as destrói no seu destrutor (`~AiAssistant`). Uma `Session` não
  existe sem o `AiAssistant` que a criou; seu ciclo de vida é controlado inteiramente pelo dono.

- **Session ◆── Message**: composição — as mensagens são armazenadas por valor dentro do
  `vector<Message>` da `Session`. Quando a `Session` é destruída, todas as suas mensagens
  são automaticamente destruídas junto. Uma `Message` não existe fora da `Session` que a contém.

### Relações de Agregação (◇)

- **AiAssistant ◇── Model**: agregação — o `AiAssistant` apenas referencia (`Model*`) um
  modelo que existe independentemente. O destrutor do assistente **não** deleta o modelo,
  pois este pode continuar existindo após o assistente ser destruído.

- **AiAssistant ◇── Tool**: agregação — o `AiAssistant` mantém ponteiros (`vector<Tool*>`)
  para ferramentas criadas externamente. O destrutor do assistente **não** deleta as
  ferramentas, que continuam existindo de forma independente.

- **Session ◇── User**: agregação — a `Session` referencia (`User*`) um usuário que existe
  independentemente. O destrutor da sessão **não** deleta o usuário, pois este pode
  participar de outras sessões ou continuar existindo após o encerramento.

## Hierarquia de Herança (TP2 — Questão 1)

- **`Tool`** é uma classe **abstrata** que define o contrato para qualquer ferramenta: `execute()` e `cost_per_call()` são métodos **puros** (virtuais sem implementação), enquanto `describe()` é **virtual não-puro** e oferece uma implementação padrão que derivadas podem estender. O **destrutor é virtual**, permitindo que derivadas executem suas lógicas de limpeza antes do destrutor da base.

- **`WebSearchTool`** é uma subclasse concreta que sobrescreve todos os métodos puros e, no método `describe()`, chama explicitamente `Tool::describe()` para reutilizar a descrição base antes de complementá-la com informações de estado (número de buscas realizadas).

- **`CalculatorTool`** é uma subclasse concreta marcada com a palavra-chave `final` (classe folha), impedindo futuras especializações. Implementa os contratos abstratos sem estender `describe()`, usando apenas a descrição padrão da base.

## Smart Pointers (Questão 4)

- **`AiAssistant::sessions_`** → `vector<unique_ptr<Session>>`: composição — o assistente é dono exclusivo das sessões, então `unique_ptr` expressa posse única e elimina `delete` manual.
- **`AiAssistant::model_`** → `shared_ptr<Model>`: agregação — o assistente compartilha a posse do modelo com outras possíveis entidades (recurso genuinamente compartilhado). O `shared_ptr` garante que o modelo não seja destruído enquanto o assistente ou outra entidade ainda o utilizar.
- **`AiAssistant::tools_`** → `vector<shared_ptr<Tool>>`: agregação — o assistente compartilha a posse das ferramentas, que podem ser reutilizadas por outros assistentes. O `shared_ptr` gerencia essa posse compartilhada.
- **`Session::user_`** → `User&` (referência): agregação — a sessão apenas observa o usuário sem possuí-lo; referência expressa observador sem posse e garante que o usuário sempre existe.
