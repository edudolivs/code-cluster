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
        -string name_
        -string description_
        -bool enabled_
        +get_name() string
        +get_description() string
        +is_enabled() bool
        +toggle() void
        +execute(string) string
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
```
