#include "Email.h"

Email::Email(string send_name, string rec_name, string tittle, string content)
    :send_name(send_name),
    rec_name(rec_name),
    tittle(tittle),
    content(content),
    read(false)
{

}

Email::Email()
    :read(false)
{
}