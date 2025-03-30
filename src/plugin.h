/*
    SPDX-FileCopyrightText: 2025 Daniele Mte90 Scasciafratte <mte90net@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KATEOLLAMAPLUGIN_H
#define KATEOLLAMAPLUGIN_H

// KF headers
#include <KTextEditor/Plugin>
#include <KTextEditor/Document>
#include <KTextEditor/MainWindow>
#include <KTextEditor/SessionConfigInterface>
#include <KTextEditor/View>
#include <KXMLGUIClient>
#include <QString>

class KateOllamaPlugin : public KTextEditor::Plugin//, public KTextEditor::SessionConfigInterface
{
    Q_OBJECT
    //_INTERFACES(KTextEditor::SessionConfigInterface)
    QString m_url;
    QString m_model;

public:
    explicit KateOllamaPlugin(QObject *parent, const QVariantList & = {});
    ~KateOllamaPlugin() override = default;

    void setUrl(QString url);
    QString url() const;
    void setModel(QString model);
    QString model() const;

    int configPages() const override;
    QObject *createView(KTextEditor::MainWindow *mainWindow) override;

    KTextEditor::ConfigPage *configPage(int number = 0, QWidget *parent = nullptr) override;
};
class KateOllamaView : public QObject, public KXMLGUIClient
{
    Q_OBJECT
public:
    explicit KateOllamaView(KateOllamaPlugin *plugin, KTextEditor::MainWindow *mainwindow);
    ~KateOllamaView() override;

private slots:
    void onSinglePrompt();
    void ollamaRequest(QString prompt);
    void printCommand();
    QString getPrompt();

private:
    KateOllamaPlugin *m_plugin = nullptr;
    KTextEditor::MainWindow *m_mainWindow = nullptr;
};
#endif // KATEOLLAMAPLUGIN_H
