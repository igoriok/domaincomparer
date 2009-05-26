#include "UrlInfo.h"

UrlInfo::UrlInfo():
        m_valid(false), m_code(0), m_length(0), m_state(UrlOk)
{
}

UrlInfo::UrlInfo(const QUrl & url, const QUrl & parent):
        m_url(url), m_parent(parent), m_code(0), m_length(0), m_state(UrlOk), m_valid(true), m_checked(false)
{
}

UrlInfo::UrlInfo(const UrlInfo & other)
{
    if (other.m_valid)
    {
        m_valid = true;
        m_url = other.m_url;
        m_parent = other.m_parent;
        m_checked = other.m_checked;
        if (m_checked)
        {
            m_code = other.m_code;
            m_type = other.m_type;
            m_length = other.m_length;
            m_state = other.m_state;
            m_desc = other.m_desc;
        }
        else
        {
            m_code = 0;
            m_type.clear();
            m_length = 0;
            m_state = UrlOk;
            m_desc.clear();
        }
    }
    else
    {
        clear();
    }
}

UrlInfo & UrlInfo::operator =(const UrlInfo & other)
{
    if (other.m_valid)
    {
        m_valid = true;
        m_url = other.m_url;
        m_parent = other.m_parent;
        m_checked = other.m_checked;
        if (m_checked)
        {
            m_code = other.m_code;
            m_type = other.m_type;
            m_length = other.m_length;
            m_state = other.m_state;
            m_desc = other.m_desc;
        }
        else
        {
            m_code = 0;
            m_type.clear();
            m_length = 0;
            m_state = UrlOk;
            m_desc.clear();
        }
    }
    else
    {
        clear();
    }
    return *this;
}

bool UrlInfo::operator ==(const UrlInfo & other) const
{
    return ((m_url.host() + m_url.path()) == (other.m_url.host() + other.m_url.path()));
}

QString UrlInfo::stateString() const
{
    QString str;
    switch(m_state)
    {
        case UrlOk:
            str.append("OK");
            break;
        case UrlWarning:
            str.append("WARNING");
            break;
        case UrlError:
            str.append("ERROR");
            break;
    }
    return str;
}

void UrlInfo::clear()
{
    m_valid = false;
    m_checked = false;
    m_url.clear();
    m_parent.clear();
    m_code = 0;
    m_type.clear();
    m_length = 0;
    m_state = UrlOk;
    m_desc.clear();
}



void UrlInfo::compare(const WebResponse & live, const WebResponse & prev)
{
    m_code = live.code();
    m_type = live.type();
    m_length = live.length();

    if (m_valid)
    {
        // comparation code
        if (prev.isNull())
        {
            switch (m_code / 100)
            {
                case 2:
                    m_state = UrlOk;
                    break;
                default:
                    m_desc = QString("Return code: %1").arg(m_code);
                    m_state = UrlWarning;
                    break;
            }
        }
        else
        {
            if (prev.code() == live.code())	{
                if ((live.code() / 100) == 2) {
        //                if ((prev.getContentType().compare(live.getContentType()) == 0) || (prev.getContentSubType().compare(live.getContentSubType()) == 0)) {
                        if (m_type.startsWith(QString("text/"))) {
                            if (live.data() == prev.data())
                                    m_state = UrlOk;
                            else {
                                    m_desc = QString("Different text: %1:%2").arg(live.data().length()).arg(prev.data().length());
                                    m_state = UrlError;
                                    // Get Extension
                             }
                        } else {
                            if (live.length() == prev.length()) {
                                    m_state = UrlOk;
                            } else {
                                    m_desc = QString("Different datasize: %1:%2").arg(live.length()).arg(prev.length());
                                    m_state = UrlError;
                            }
                        }
        //                } else {
        //                    m_desc = QString("Different type: %1:%2").arg(live.getContentType()).arg(prev.getContentType());
        //                    m_ok = false;
        //                }
                } else {
                    m_desc = QString("Both code: %1").arg(live.code());
                    m_state = UrlOk;
                }
            } else {
                m_desc = QString("Different code: %1:%2").arg(live.code()).arg(prev.code());
                m_state = UrlError;
            }
        }
    }

    m_checked = true;
}

uint qHash(const UrlInfo & ui)
{
    return qHash(ui.m_url.host() + ui.m_url.path());
}

uint qHash(const QUrl & url)
{
    return qHash(url.toString());
}
