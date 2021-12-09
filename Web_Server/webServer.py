from flask import Flask, render_template

mos = 0
tmp1 = 0
tmp2 = 0
hum1 = 0
hum2 = 0
lux = 0

app = Flask(__name__)

@app.route('/tmp/<name>')
def hello_tmp(name):
	global tmp1
	tmp1=name
	return '현재온도 %s 도' % tmp1

@app.route('/tmpUp')
def tmpUp():
	global tmp1
	tmp1+=1
	return '현재온도 %s도'% tmp1

@app.route('/hum/<name>')
def hello_hum(name):
	print( '습도값 %s' % name)
	return '현재습도  %s' % name

@app.route('/lux/<name>')
def hello_lux(name):
	return '현재조도 %s lux' % name

@app.route('/grd/<name>')
def hello_grd(name):
	return '현재토양 %s 정도' % name

@app.route('/control/<name>')
def hello_control(name):
	return 'control %s' % tmp1

@app.route('/')
def home():
	return render_template("home.html", name1=mos, name2=tmp1,name3=hum1)

@app.route('/<mosIn>/<tmpIn>/<humIn>')
def home2(mosIn,tmpIn,humIn):
	mos = mosIn
	tmp1 = tmpIn
	hum1 = humIn
	return render_template("home.html",name1=mos,name2=tmp1,name3=hum1)


if __name__ == '__main__':
	app.run(host='0.0.0.0', port=80)
