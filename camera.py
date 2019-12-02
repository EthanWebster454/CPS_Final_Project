import smtplib,ssl  
from picamera import PiCamera  
from time import sleep  
from email.mime.multipart import MIMEMultipart  
from email.mime.base import MIMEBase  
from email.mime.text import MIMEText  
from email.utils import formatdate  
from email import encoders  
from sys import argv

camera = PiCamera()  

camera.start_preview()  
sleep(5)  
camera.capture('/home/aabdelhamid/project/image.jpg')     # image path set
sleep(5)  
camera.stop_preview()  
def send_an_email():  
    toaddr = 'amr.atef91@gmail.com'      # To id 
    me = 'amr.atef91@gmail.com'          # your id
    subject = argv[1]              # Subject
  
    msg = MIMEMultipart()  
    msg['Subject'] = subject  
    msg['From'] = me  
    msg['To'] = toaddr  
    msg.preamble = "test "   
    #msg.attach(MIMEText(text))  
  
    part = MIMEBase('application', "octet-stream")  
    part.set_payload(open("image.jpg", "rb").read())  
    encoders.encode_base64(part)  
    part.add_header('Content-Disposition', 'attachment; filename="image.jpg"') $
    msg.attach(part)  
  
    try:  
       s = smtplib.SMTP('smtp.gmail.com', 587)  # Protocol
       s.ehlo()  
       s.starttls()  
       s.ehlo()  
       s.login(user = 'amr.atef91@gmail.com', password = '*******')  # User id $
       #s.send_message(msg)  
       s.sendmail(me, toaddr, msg.as_string())  
       s.quit()  
    #except:  
    #   print ("Error: unable to send email")    
    except SMTPException as error:  
          print ("Error")                # Exception
  
send_an_email()
