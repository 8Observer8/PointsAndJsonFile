#include <QVector>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_nrows( 10 ),
    m_ncols( 2 ),
    m_model( m_nrows, m_ncols, this )
{
    ui->setupUi(this);
    this->setFixedSize( this->size() );

    // Set Header
    m_model.setHeaderData( 0, Qt::Horizontal, "x" );
    m_model.setHeaderData( 1, Qt::Horizontal, "y" );

    ui->tableView->setModel( &m_model );

    QModelIndex index;
    for( int row = 0; row < m_nrows; ++row ) {
        for( int col = 0; col < m_ncols; ++col ) {
            index = m_model.index( row, col );
            m_model.setData( index, QString::number( row + 1 ) + ".0" );
        }
    }

    connect( &m_model, SIGNAL( dataChanged( QModelIndex, QModelIndex ) ),
             this, SLOT( slotDrawPlot( QModelIndex, QModelIndex ) ) );

    slotDrawPlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotDrawPlot(const QModelIndex &index1,
                              const QModelIndex &index2)
{
    QVector<double> x( m_nrows ), y( m_nrows ); // initialize with entries 0..100

    QModelIndex index;
    for( int row = 0; row < m_nrows; ++row ) {
        double value;
        bool ok = true;

        // x
        index = m_model.index( row, 0 );
        value = m_model.data( index ).toDouble( &ok );
        if ( ok ) {
            x[row] = value;
        } else {
            QMessageBox::critical( this, tr( "Error" ), tr( "Error: you have incorrect value in the table." ) );
            return;
        }

        // y
        index = m_model.index( row, 1 );
        value = m_model.data( index ).toDouble( &ok );
        if ( ok ) {
            y[row] = value;
        } else {
            QMessageBox::critical( this, tr( "Error" ), tr( "Error: you have incorrect value in the table." ) );
            return;
        }
    }

    // Create graph and assign data to it:
    ui->plotWidget->addGraph();
    ui->plotWidget->graph( 0 )->setData( x, y );

    // Give the axes some labels:
    ui->plotWidget->xAxis->setLabel( "x" );
    ui->plotWidget->yAxis->setLabel( "y" );

    // Set axes ranges, so we see all data:
    ui->plotWidget->xAxis->setRange( 0, 10 );
    ui->plotWidget->yAxis->setRange( 0, 10 );

    ui->plotWidget->replot();
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr( "Open File" ),
                QString(),
                tr( "Json Files (*.json)" ) );

    if ( fileName.isEmpty() ) {
        return;
    }

    // Open the file
    QFile file( fileName );
    if ( !file.open( QIODevice::ReadOnly ) ) {
        QMessageBox::critical(
                    this, tr( "Error" ),
                    tr( "Error: unable to open the file \"%1\"" ).arg( fileName ) );
        return;
    }

    // Read the file
    QByteArray data = file.readAll();

    // Create Json document
    QJsonDocument document;
    document = document.fromJson( data );

    // Get the points from the Json document
    QModelIndex index;
    QString x, y;
    for( std::size_t row = 0; row < m_nrows; ++row ) {
        x = document.object()[QString( "x[%1]" ).arg( row )].toString();
        index = m_model.index( row, 0 );
        m_model.setData( index, x );

        y = document.object()[QString( "y[%1]" ).arg( row )].toString();
        index = m_model.index( row, 1 );
        m_model.setData( index, y );
    }

    slotDrawPlot();
}

void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(
                this,
                tr( "Save File" ),
                QString(),
                tr( "Json Files (*.json)" ) );

    if ( fileName.isEmpty() ) {
        return;
    }

    // Create a Json object from the table
    QModelIndex index;
    QJsonObject json;
    QString value;
    for ( std::size_t row = 0; row < m_nrows; ++row ) {
        index = m_model.index( row, 0 );
        value = m_model.data( index ).toString();
        json[QString("x[%1]").arg( row )] = value;

        index = m_model.index( row, 1 );
        value = m_model.data( index ).toString();
        json[QString("y[%1]").arg( row )] = value;
    }

    // Create a Json document
    QJsonDocument document;
    document.setObject( json );

    // Write the Json document to a file
    // Open a file
    QFile file( fileName );
    if ( !file.open( QIODevice::WriteOnly ) ) {
        QMessageBox::critical(
                    this, tr( "Error" ),
                    tr( "Error: unable to open the file \"%1\"" ).arg( fileName ) );
        return;
    }

    // Write to the file
    if ( !file.write( document.toJson() ) ) {
        QMessageBox::critical(
                    this, tr( "Error" ),
                    tr( "Error: unable to write to the file \"%1\"" ).arg( fileName ) );
        return;
    }
}

void MainWindow::on_actionExit_triggered()
{
    this->close();
}
