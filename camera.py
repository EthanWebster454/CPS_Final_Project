# sourced from https://www.raspberrypi.org/forums/viewtopic.php?t=123555
import smtplib,ssl 
from picamera import PiCamera  
from PIL import Image
from time import sleep  
from email.mime.multipart import MIMEMultipart  
from email.mime.base import MIMEBase  
from email.mime.text import MIMEText  
from email.utils import formatdate  
from email import encoders  
from sys import argv

emailAddr = 'tyrantsthrall@gmail.com'


# snap a photo for verification if the sensor detects possible fever state
if argv[1] == "remote":
   
   camera = PiCamera()  
   camera.start_preview()  
   sleep(1)  
   camera.capture('/home/ethan/project/image.jpg')     # image path set
   sleep(1) 
   camera.stop_preview()

   # rotate the image
   origImg  = Image.open("image.jpg") 
   rotImg = origImg.transpose(Image.ROTATE_270)
   rotImg.save('image.jpg', 'JPEG')


def send_an_email():  

    toaddr = emailAddr      # To id 
    me = emailAddr          # your id

   # Subject
    if argv[1] == 'room':
       subject = 'The room temperature is in the danger zone'              
    else:
       subject = 'The monitoring system may have detected a fever'
  
    msg = MIMEMultipart()  
    msg['Subject'] = subject  
    msg['From'] = me  
    msg['To'] = toaddr  
    msg.preamble = "test "   
    #msg.attach(MIMEText(text))  
  
    if argv[1] == "remote":
      part = MIMEBase('application', "octet-stream")  
      part.set_payload(open("image.jpg", "rb").read())  
      encoders.encode_base64(part)  
      part.add_header('Content-Disposition', 'attachment; filename="capture.jpg"')
      msg.attach(part)  
  
    try:  
       s = smtplib.SMTP('smtp.gmail.com', 587)  # Protocol
       s.ehlo()  
       s.starttls()  
       s.ehlo()  
       s.login(user = emailAddr, password = '*******')  # User id $
       #s.send_message(msg)  
       s.sendmail(me, toaddr, msg.as_string())  
       s.quit()  
    #except:  
    #   print ("Error: unable to send email")    
    except SMTPException as error:  
          print ("Error sending mail")                # Exception
  

send_an_email()
