#pragma warning (disable: 4703)
#pragma warning (disable: 4700)
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <string>
#include <iostream>
#include <sql.h>
#include <sqlext.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <stdlib.h>
#include <sal.h>


#pragma comment( lib, "odbc32.lib" )

#define BUFFER_SIZE 256

/*
    DataSource: dsn_system_northwind
    DataBase: northwind
    Server: localhost
    UserName: postgres
    port: 5433
    password: anton220200

*/

SQLHENV environmentHandle;
SQLHDBC connectionHandle;
SQLRETURN ret;

void outputError(SQLHANDLE handle, SQLSMALLINT handleType) {
    SQLCHAR sqlState[6];
    SQLINTEGER nativeError;
    SQLCHAR messageText[BUFFER_SIZE];
    SQLSMALLINT textLength;

    setlocale(LC_ALL, "Russian");

    SQLGetDiagRecA(handleType, handle, 1, sqlState, &nativeError, messageText, sizeof(messageText), &textLength);
    std::cout << "Error message: " << messageText << std::endl;
}

typedef struct
{
    std::string DataSource;
    std::string sql;

}entry_param;

SQLHDBC try_connect_to_db(SQLCHAR* dsn_name)
{
    // Allocate an environment handle
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &environmentHandle);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        std::cout << "Failed to allocate an environment handle." << std::endl;
        return 0x00;
    }

    // Set the ODBC version to use
    ret = SQLSetEnvAttr(environmentHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        std::cout << "Failed to set the ODBC version." << std::endl;
        return 0x00;
    }

    // Allocate a connection handle
    ret = SQLAllocHandle(SQL_HANDLE_DBC, environmentHandle, &connectionHandle);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
    {
        std::cout << "Failed to allocate a connection handle." << std::endl;
        return 0x00;
    }

    //DSN=dsn_system_northwind;
    //char* name = "DSN=dsn_system_northwind;";
    
    ret = SQLDriverConnectA(connectionHandle, NULL, dsn_name, SQL_NTS,
        NULL, 0, NULL, SQL_DRIVER_COMPLETE);

    if (ret != SQL_SUCCESS)
    {
        std::cout << "FAILED SQLDriverConnectA." << std::endl;

        outputError(connectionHandle, SQL_HANDLE_DBC);
        SQLFreeHandle(SQL_HANDLE_DBC, connectionHandle);
        SQLFreeHandle(SQL_HANDLE_ENV, environmentHandle);

        return 0x00;
    }

    std::cout << "Connected to PostgreSQL!" << std::endl;
    return connectionHandle;
}
void send_sql(SQLCHAR* sql_request)
{
    SQLHSTMT stmt;

    SQLAllocHandle(SQL_HANDLE_STMT, connectionHandle, &stmt);
    //"select * from employees"
    ret = SQLExecDirectA(stmt, sql_request, SQL_NTS);


    if (ret != SQL_SUCCESS)
    {
        std::cout << "FAILED SQLTablesA." << std::endl;

        outputError(connectionHandle, SQL_HANDLE_DBC);
        SQLFreeHandle(SQL_HANDLE_DBC, connectionHandle);
        SQLFreeHandle(SQL_HANDLE_ENV, environmentHandle);
    }

    SQLSMALLINT columns;
    ret = SQLNumResultCols(stmt, &columns);
    if (ret != SQL_SUCCESS)
    {
        std::cout << "Failed SQLNumResultCols." << std::endl;

    }

    int row = 0;
    while (SQL_SUCCEEDED(ret = SQLFetch(stmt)))
    {

        std::cout << "SUCCESS SQLFetch." << std::endl;


        SQLLEN cbLeft;
        printf("Row %d\n", row++);
        /* Loop through the columns */
        for (SQLUSMALLINT i = 1; i <= columns; i++)
        {
            //SQLINTEGER indicator;
            char buf[512];
            /* retrieve column data as a string */
            ret = SQLGetData(stmt, i, SQL_C_CHAR,
                buf, sizeof(buf), &cbLeft);

            if (SQL_SUCCEEDED(ret))
            {
                /* Handle null columns */
                //strcpy(buf, "NULL");
                printf("  Column %u : %s\n", i, buf);
            }
        }
    }
}
void try_disconnect_from_db()
{
    // Disconnect from the database
    ret = SQLDisconnect(connectionHandle);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Failed to disconnect from PostgreSQL." << std::endl;
    }

    // Free the connection handle
    ret = SQLFreeHandle(SQL_HANDLE_DBC, connectionHandle);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Failed to free the connection handle." << std::endl;
    }

    // Free the environment handle
    ret = SQLFreeHandle(SQL_HANDLE_ENV, environmentHandle);
    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        std::cout << "Failed to free the environment handle." << std::endl;
    }
}
int main() 
{

    setlocale(LC_ALL, "Russian");
    entry_param my_entry;

    std::cout << "¬вудите dsn им€(<Data Source>):" << std::endl;
    std::getline(std::cin, my_entry.DataSource);

    SQLHDBC connectionHandle = try_connect_to_db((SQLCHAR*)my_entry.DataSource.c_str());

    std::cout << "¬вудите SQL запрос:" << std::endl;
    std::getline(std::cin, my_entry.sql);

    send_sql((SQLCHAR*)my_entry.sql.c_str());
    try_disconnect_from_db();

    return 0;
}