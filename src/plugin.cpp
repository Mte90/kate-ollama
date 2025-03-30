/*
    SPDX-FileCopyrightText: 2025 Daniele Mte90 Scasciafratte <mte90net@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugin.h"
#include "settings.h"

// KF headers
#include <KTextEditor/MainWindow>
#include <KTextEditor/View>
#include <KTextEditor/Document>
#include <KTextEditor/Plugin>
#include <KTextEditor/Application>
#include <KTextEditor/Editor>

#include <QAction>
#include <KActionCollection>
#include <KXMLGUIClient>
#include <KLocalizedString>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QAction>
#include <QString>
#include <QRegularExpression>
#include <KPluginFactory>
#include <KXMLGUIFactory>
#include <KLocalizedString>
#include <QDebug>
#include <QMenu>

using namespace Qt::Literals::StringLiterals;

K_PLUGIN_FACTORY_WITH_JSON(KateOllamaFactory, "kateollama.json", registerPlugin<KateOllamaPlugin>();)


KateOllamaPlugin::KateOllamaPlugin(QObject* parent, const QVariantList&)
    : KTextEditor::Plugin(parent)
{
}

int KateOllamaPlugin::configPages() const
{
    return 1;
}

void KateOllamaPlugin::setModel(QString model)
{
    m_model = model;
}

void KateOllamaPlugin::setUrl(QString url)
{
    m_url = url;
}

QString KateOllamaPlugin::model() const
{
    return m_model;
}

QString KateOllamaPlugin::url() const
{
    return m_url;
}

KateOllamaView::KateOllamaView(KateOllamaPlugin *plugin, KTextEditor::MainWindow *mainwindow)
    : KXMLGUIClient()
    , m_plugin(plugin)
    , m_mainWindow(mainwindow)
{
    KXMLGUIClient::setComponentName(u"kateollama"_s, i18n("Kate-Ollama"));

    auto ac = actionCollection();
    QAction *a = ac->addAction(QStringLiteral("kateollama"));
    a->setText(i18n("Run Ollama"));
    a->setIcon(QIcon::fromTheme(QStringLiteral("debug-run")));
    KActionCollection::setDefaultShortcut(a, QKeySequence((Qt::CTRL | Qt::Key_Semicolon)));
    connect(a, &QAction::triggered, this, &KateOllamaView::onSinglePrompt);

    QAction *a2 = ac->addAction(QStringLiteral("kateollama-command"));
    a2->setText(i18n("Add Ollama Command"));
    KActionCollection::setDefaultShortcut(a2, QKeySequence((Qt::CTRL | Qt::Key_Slash)));
    connect(a2, &QAction::triggered, this, &KateOllamaView::printCommand);

    m_mainWindow->guiFactory()->addClient(this);
}

void KateOllamaView::printCommand()
{
    KTextEditor::View *view = m_mainWindow->activeView();;
    if (view) {
        KTextEditor::Document *document = view->document();
        QString text = document->text();
        KTextEditor::Cursor cursor = view->cursorPosition();
        document->insertText(cursor, "// AI: ");
    }
}

void KateOllamaView::ollamaRequest(QString prompt) {
    KTextEditor::View *view = m_mainWindow->activeView();
    KTextEditor::Document *document = view->document();
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkRequest request(QUrl(m_plugin->url() + "/api/generate"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json.insert("model", m_plugin->model());
    json.insert("prompt", prompt);
    QJsonDocument doc(json);

    QNetworkReply *reply = manager->post(request, doc.toJson());

    connect(reply, &QNetworkReply::metaDataChanged, this, [=]() {
            KTextEditor::Cursor cursor = view->cursorPosition();
            document->insertText(cursor, "\n");
    });

    connect(reply, &QNetworkReply::readyRead, this, [=]() {
            QString responseChunk = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseChunk.toUtf8());
            QJsonObject jsonObj = jsonDoc.object();

            if (jsonObj.contains("response")) {
                QString responseText = jsonObj["response"].toString();

                KTextEditor::Cursor cursor = view->cursorPosition();
                document->insertText(cursor, responseText);
            }
    });

    connect(reply, &QNetworkReply::finished, this, [=]() {
            reply->deleteLater();

            KTextEditor::Cursor cursor = view->cursorPosition();
            document->insertText(cursor, "\n");
    });
}

QString KateOllamaView::getPrompt() {
    KTextEditor::View *view = m_mainWindow->activeView();
    KTextEditor::Document *document = view->document();
    QString text = document->text();

    QRegularExpression re("// AI:(.*)");
    QRegularExpressionMatchIterator matchIterator = re.globalMatch(text);

    if (matchIterator.hasNext()) {
        QRegularExpressionMatch match = matchIterator.next();
        return match.captured(1).trimmed();
    }

    return {};
}

void KateOllamaView::onSinglePrompt()
{
    KTextEditor::View *view = m_mainWindow->activeView();
    if (view) {
        QString prompt = KateOllamaView::getPrompt();
        if (!prompt.isEmpty()) {
            KateOllamaView::ollamaRequest(prompt);
        }
    }
}

KateOllamaView::~KateOllamaView()
{
    m_mainWindow->guiFactory()->removeClient(this);
}

QObject* KateOllamaPlugin::createView(KTextEditor::MainWindow* mainwindow)
{
    return new KateOllamaView(this, mainwindow);
}

KTextEditor::ConfigPage *KateOllamaPlugin::configPage(int number, QWidget *parent)
{
    if (number != 0) {
        return nullptr;
    }
    return new KateOllamaConfigPage(parent, this);
}

#include <plugin.moc>
