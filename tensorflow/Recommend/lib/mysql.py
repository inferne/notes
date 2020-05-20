import pymysql.cursors

class Mysql(object):
	def __init__(self, config):
		self._config = config

	def connect(self):
		self._connection = pymysql.connect(
						host=self._config['host'],
						user=self._config['user'],
						password=self._config['pass'],
						db=self._config['name'],
						port=self._config['port'],
						cursorclass=pymysql.cursors.DictCursor)
	def select(self, sql):
		with self._connection.cursor() as cursor:
			cursor.execute(sql)
			result = cursor.fetchone()
			return result
	def select_list(self, sql):
		with self._connection.cursor() as cursor:
			cursor.execute(sql)
			result = cursor.fetchall()
			return result