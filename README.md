#C++ Coding Challenge Submission from Yang Chi (yang@yangchi.me)

## To compile:

make

### Dependencies:

mysql++-devel

ldns-devel

g++ with C++11 support

The MYSQLPP\_MYSQL\_HEADERS\_BURIED is defined at the beginning of db.h. This is for the common case that 'mysql' and 'mysql++' headers are in separate directories in the include path. Otherwise this should not be defined.

## DB setup:

The program will read database setup info from dbconfig.in. It will use the username and password provided in that file to connect to MySQL. So for this program to run correctly, either change these information, or craete such user in your MySQl.

The default database name is 'dnslatency'. If this database does not exist in your MySQL, it will be created by the program. You can change this name in the dbconfig.in as well. Note that the program will *NOT* create the user for you. So create the user first, or change the user info in the dbconfig.in first. And grant proper privileges to this user. Then the program will be able to create the database.

## To run:

./query --freq frequency

The unit of the frequency is ms. So e.g. './query --freq 2000ms' will make the program query the domains every 2000ms. Note that for now the unit specified by user will be ignores. Thus './query --freq 2000ns' will still query every 2000ms.

The program will read domains from domains.in and will query every one of them in each round for 10 rounds. For now the round number is hard coded in the program itself.

At the end, the program will print out stats including mean, std dev, etc.

## More on DB:

The program will create 2 tables: latency table for all recorded query latencies, and stats table for stats and timestamps info.

Apparently the mean and standard deviation values of latencies can be calculated directly by this query in MySQL:

'SELECT AVG(latency), STDDEV\_POP(latency) from latency group by domains;'

and this will make the stats table seem unnecessary.

However since the question itself requires the code to "keep track in db stats per domain", I created the stats table.

latency table:

+---------+-------------+------+-----+---------+-------+

| Field   | Type        | Null | Key | Default | Extra |

+---------+-------------+------+-----+---------+-------+

| domains | varchar(20) | NO   |     | NULL    |       |

| latency | bigint(20)  | NO   |     | NULL    |       |

+---------+-------------+------+-----+---------+-------+

stats table:

+---------+-------------+------+-----+---------+-------+

| Field   | Type        | Null | Key | Default | Extra |

+---------+-------------+------+-----+---------+-------+

| domains | varchar(20) | NO   | PRI | NULL    |       |

| mean    | double      | YES  |     | 0       |       |

| dev     | double      | YES  |     | 0       |       |

| count   | int(11)     | YES  |     | 0       |       |

| first   | timestamp   | YES  |     | NULL    |       |

| last    | timestamp   | YES  |     | NULL    |       |

+---------+-------------+------+-----+---------+-------+

