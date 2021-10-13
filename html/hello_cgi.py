#!/usr/bin/python

# Import CGI and CGIT module
import cgi, cgitb

# to create instance of FieldStorage
# class which we can use to work
# with the submitted form data
form = cgi.FieldStorage()
name = form.getvalue('name')

print("<html>")
print("<head>")
print("<title>Hello CGI ! Python script</title>")
print("</head>")
print("<body>")
print(f"<h2>Hello, {name} !</h2>")
print("</body>")
print("</html>")
