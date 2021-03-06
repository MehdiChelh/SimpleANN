#include "quandldialog.h"

QuandlDialog::QuandlDialog(Data* _data, TableWidget* _table, QComboBox* _comboBox_1, QComboBox* _comboBox_2) : QDialog()
{
    data = _data;
    table = _table;
    comboBox_1 = _comboBox_1;
    comboBox_2 = _comboBox_2;

    this->setWindowTitle(QString("Download financial time serie from Quandl"));
    this->resize(500, 500*9/16);

    grid = new QGridLayout();
    this->setLayout(grid);

    searchKeywords = new QLineEdit(this);
    searchKeywords -> setPlaceholderText("Ex: Apple");
    grid->addWidget(searchKeywords, 0,0);

    search_button = new QPushButton("Search", this);
    search_button->setCursor(Qt::PointingHandCursor);
    QObject::connect(search_button, SIGNAL(clicked()), this, SLOT(enablingSelection()));
    grid->addWidget(search_button, 0, 1);

    listWidget = new QTreeWidget(this);
    listWidget->setColumnCount(4);
    QStringList columnsLabel;
    columnsLabel << "Name" << "ID" << "Oldest date" << "Newest date";
    listWidget->setHeaderLabels(columnsLabel);
    listWidget->resize(500, 200);
    grid->addWidget(listWidget, 1, 0, 1, 2);

    select_dataset = new QPushButton("Select this time serie", this);
    select_dataset->setCursor(Qt::PointingHandCursor);
    grid->addWidget(select_dataset, 2, 0, 1, 2);

    search = new QuandlSearch(searchKeywords, listWidget);
    QObject::connect(search_button, SIGNAL(clicked()), search, SLOT(searchResponse()));
    QObject::connect(select_dataset, SIGNAL(clicked()), this, SLOT(selectDate()));


    this->show();
}

void QuandlDialog::selectDate()
{
    if(listWidget->currentItem() != NULL){
        QTreeWidgetItem* item = listWidget->currentItem();

        listWidget->setDisabled(true);
        select_dataset->setDisabled(true);

        QLabel* label_start_date = new QLabel("Start date", this);
        grid->addWidget(label_start_date, 4, 0);
        start_date = new QLineEdit(this);
        start_date->setText(item->text(2));
        grid->addWidget(start_date, 4, 1);

        QLabel* label_end_date = new QLabel("End date", this);
        grid->addWidget(label_end_date, 3, 0);
        end_date = new QLineEdit(this);
        end_date->setText(item->text(3));
        grid->addWidget(end_date, 3, 1);

        destination_folder = new QLineEdit(this);
        destination_folder->setPlaceholderText("Destination folder for the dataset");
        grid -> addWidget(destination_folder, 5, 0);

        QPushButton* choose_folder_button = new QPushButton("Choose folder", this);
        choose_folder_button->setCursor(Qt::PointingHandCursor);
        QObject::connect(choose_folder_button, SIGNAL(clicked()), this, SLOT(chooseFolder()));
        grid->addWidget(choose_folder_button, 5, 1);



        QPushButton* download_button = new QPushButton("Download data", this);
        download_button->setCursor(Qt::PointingHandCursor);
        QObject::connect(download_button, SIGNAL(clicked()), this, SLOT(download()));
        grid->addWidget(download_button, 6, 0, 1, 2);
    }


}

void QuandlDialog::download()
{
    QString pathToCSV = search->downloadDataset(start_date->text(), end_date->text(), destination_folder->text());
    this->close();
    QList<QByteArray> columns = data->getColumnsOfCSV(pathToCSV);
    QList<QString> col = Data::byteArraysToStrings(columns);
    QList<QString> inputColumns = Tab1::selectItemsDialog("Select input labels", "Select the label(s) that you want to use as input variables in your model", col);
    inputColumns.removeAll("Date");
    QList<QString> outputColumns = Tab1::selectItemsDialog("Select output labels", "Select the label(s) that you want to use as output variables in your model", col);
    outputColumns.removeAll("Date");

    if(outputColumns.length() >= 1 && inputColumns.length() >= 1){
        data->openCSV(pathToCSV, inputColumns, "Date", outputColumns);
        table->fill(data);
        comboBox_1->clear();
        comboBox_1->addItems(inputColumns);
        comboBox_2->clear();
        comboBox_2->addItems(inputColumns);
    }
}

void QuandlDialog::chooseFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select a destination folder for downloading data"),
                                   "",
                                   QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    destination_folder->setText(dir);
}

void QuandlDialog::enablingSelection()
{
    listWidget->setEnabled(true);
    select_dataset->setEnabled(true);
}

QuandlDialog::~QuandlDialog()
{
    delete listWidget;
    delete grid;
}
