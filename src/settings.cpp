/*
 *  SPDX-FileCopyrightText: 2025 Daniele Mte90 Scasciafratte <mte90net@gmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "settings.h"
#include "plugin.h"
#include <KConfigGroup>
#include <KSharedConfig>
#include <KLocalizedString>
#include <KTextEditor/ConfigPage>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVBoxLayout>
#include <qobject.h>

KateOllamaConfigPage::KateOllamaConfigPage(QWidget *parent, KateOllamaPlugin *plugin)
    : KTextEditor::ConfigPage(parent)
    , m_plugin(plugin)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    comboBox = new QComboBox(this);
    QObject::connect(comboBox, &QComboBox::currentIndexChanged, this, &KateOllamaConfigPage::changed);
    layout->addWidget(comboBox);

    lineEdit = new QLineEdit(this);
    QObject::connect(lineEdit, &QLineEdit::textEdited, this, &KateOllamaConfigPage::changed);
    layout->addWidget(lineEdit);

    setLayout(layout);
    loadSettings();
}

void KateOllamaConfigPage::fetchModelList()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, [this](QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);

            if (jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                if (jsonObj.contains("models") && jsonObj["models"].isArray()) {
                    QJsonArray modelsArray = jsonObj["models"].toArray();
                    for (const QJsonValue &modelValue : modelsArray) {
                        if (modelValue.isString()) {
                            comboBox->addItem(modelValue.toString());
                        }
                    }
                }
            }
        }
        reply->deleteLater();
    });

    QUrl url(lineEdit->text() + "/models");
    QNetworkRequest request(url);
    manager->get(request);
}

QString KateOllamaConfigPage::name() const
{
    return i18n("Ollama");
}

QString KateOllamaConfigPage::fullName() const
{
    return i18nc("Groupbox title", "Ollama Settings");
}

QIcon KateOllamaConfigPage::icon() const
{
    return QIcon::fromTheme(QLatin1String("project-open"), QIcon::fromTheme(QLatin1String("view-list-tree")));
}

void KateOllamaConfigPage::defaults()
{
    comboBox->setCurrentText("llama3.2:latest");
    lineEdit->setText("http://localhost:11434");
    m_plugin->setUrl(lineEdit->text());
    m_plugin->setModel(comboBox->currentText());
}

void KateOllamaConfigPage::reset()
{
    loadSettings();
    m_plugin->setUrl(lineEdit->text());
    m_plugin->setModel(comboBox->currentText());
}

void KateOllamaConfigPage::apply()
{
    saveSettings();
    m_plugin->setUrl(lineEdit->text());
    m_plugin->setModel(comboBox->currentText());
}

void KateOllamaConfigPage::saveSettings()
{
    KConfigGroup group(KSharedConfig::openConfig(), "KateOllama");
    group.writeEntry("Model", comboBox->currentText());
    group.writeEntry("SystemPrompt", lineEdit->text());
    group.sync();
}

void KateOllamaConfigPage::loadSettings()
{
    KConfigGroup group(KSharedConfig::openConfig(), "KateOllama");
    lineEdit->setText(group.readEntry("SystemPrompt", "http://localhost:11434"));
    fetchModelList();
    comboBox->setCurrentText(group.readEntry("Model", "llama3.2:latest"));
}
