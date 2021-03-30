Поднять сервер server.lua с помощью команды:
tarantool -i server.lua <port>

Поднять балансировщик balancer.lua в интерактивном режиме с помощью команды:
tarantool -i balancer.lua

Добавить сервер в balancer.lua с помощью функции:
add_connection("admin:test@localhost:3034")

Удалить сервер по номеру в массиве с помощью функции:
remove_connection(conn_num)