#include "quandlsearch.h"

QuandlSearch::QuandlSearch() : QDialog()
{
    this->setWindowTitle(QString("Selection des données provenant de Quandl"));
    grid = new QGridLayout();
    this->setLayout(grid);

    searchKeywords = new QLineEdit(this);
    searchKeywords -> setPlaceholderText("");
    grid->addWidget(searchKeywords, 0,0);

    search_button = new QPushButton(this);
    search_button->setText(QString("Rechercher"));
    search_button->setCursor(Qt::PointingHandCursor);
//    QObject::connect(search_button, SIGNAL(clicked()), this, SLOT(disableButtons()));
    QObject::connect(search_button, SIGNAL(clicked()), this, SLOT(searchResponse()));
    grid->addWidget(search_button, 0, 1);

    listWidget = new QTreeWidget(this);
    listWidget->resize(500, 200);
    grid->addWidget(listWidget, 1, 0, 1, 2);

    select_dataset = new QPushButton(this);
    select_dataset->setText("Selectionner");
    select_dataset->setCursor(Qt::PointingHandCursor);
    QObject::connect(select_dataset, SIGNAL(clicked()), this, SLOT(selectItem()));
    grid->addWidget(select_dataset, 2, 0, 1, 2);

    this->show();
}

//Sert a extraire les données renvoyer par l'appel à libcurl
QString getResp;
size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up)
{ //callback must have this declaration
    //buf is a pointer to the data that curl has for us
    //size*nmemb is the size of the buffer

    for (int c = 0; c<size*nmemb; c++)
    {
        getResp.push_back(buf[c]);
    }
    return size*nmemb; //tell curl how many bytes we handled
}


void QuandlSearch::searchResponse()
{
    qDebug() << searchKeywords->text();
    listWidget->clear();

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        qDebug() << "https://www.quandl.com/api/v3/datasets.json?query="+searchKeywords->text();
        std::string url = std::string("https://www.quandl.com/api/v3/datasets.json?api_key=kuExaMxAa629HY7dRvgH&"
                                      "database_code=WIKI&per_page=5&page=1&query=")
                                        + std::string(searchKeywords->text().toUtf8().constData());
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }

    //On remplit la liste des résultats affichés
    QJsonDocument doc = QJsonDocument::fromJson(getResp.toUtf8());
    QJsonArray jsonArray = doc.object()["datasets"].toArray();
    foreach (const QJsonValue & value, jsonArray) {
        QJsonObject obj = value.toObject();
        QList<QString> list;
        list << obj["name"].toString() << obj["dataset_code"].toString() << obj["oldest_available_date"].toString() << obj["newest_available_date"].toString();
        qDebug() << list;
        listWidget->addTopLevelItem(new QTreeWidgetItem(list));
    }
    getResp = "";

}

void QuandlSearch::selectItem()
{
    qDebug() << (*(listWidget->currentItem())).text(1);
    QString dataset_code = (*(listWidget->currentItem())).text(1);

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if(curl) {
        qDebug() << "https://www.quandl.com/api/v3/datasets.json?query="+searchKeywords->text();
        std::string url = std::string("https://www.quandl.com/api/v3/datasets/WIKI/")
                                      + std::string(dataset_code.toUtf8().constData())
                                      + std::string(".json?api_key=kuExaMxAa629HY7dRvgH");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));

        curl_easy_cleanup(curl);
    }
    QJsonDocument doc = QJsonDocument::fromJson(getResp.toUtf8());
    qDebug() << doc.object();
    getResp = "";

}

void QuandlSearch::disableButtons()
{
    select_dataset->setDisabled(true);
    search_button->setDisabled(true);
}

QuandlSearch::~QuandlSearch()
{
    delete listWidget;
    delete grid;
}