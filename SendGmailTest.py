# Send an HTML email with an embedded image and a plain text message for
# email clients that don't want to display the HTML.
import smtplib
from email.mime.text import MIMEText
from email.mime.image import MIMEImage
from email.mime.multipart import MIMEMultipart

# # Define these once; use them twice!
# strFrom = 'info.enchanted.oc@gmail.com'
# # strTo = '7142445507@tmomail.net'
# strTo = 'vulevu121@gmail.com'
#
# # Create the root message and fill in the from, to, and subject headers
# msgRoot = MIMEMultipart('related')
# msgRoot['Subject'] = '[Enchanted Mirror] Here are your photos!'
# msgRoot['From'] = strFrom
# msgRoot['To'] = strTo
# msgRoot.preamble = 'This is a multi-part message in MIME format.'
#
# # Encapsulate the plain and HTML versions of the message body in an
# # 'alternative' part, so message agents can decide which they want to display.
# msgAlternative = MIMEMultipart('alternative')
# msgRoot.attach(msgAlternative)
#
# msgText = MIMEText('Thank you for using the Enchanted Mirror :) Check out www.facebook.com/enchantedphotoboothoc/')
# msgAlternative.attach(msgText)

# s = smtplib.SMTP_SSL('smtp.gmail.com', 465)
# s.login('info.enchanted.oc@gmail.com', 'takemetonewheights$2020')

# This example assumes the image is in the current directory

with open('Email.txt') as f:
    emailList = eval(f.read())

    with smtplib.SMTP_SSL('smtp.gmail.com', 465) as s:
        s.login('info.enchanted.oc@gmail.com', 'takemetonewheights$2020')

        for e in emailList:
            email = e['Email']
            print('Sending to', email)
            
            strTo = email
            strFrom = 'info.enchanted.oc@gmail.com'
            msgRoot = MIMEMultipart('related')
            msgRoot['Subject'] = '[Enchanted Mirror] Here are your photos!'
            msgRoot['From'] = strFrom
            msgRoot['To'] = strTo
            msgRoot.preamble = 'This is a multi-part message in MIME format.'
            msgAlternative = MIMEMultipart('alternative')
            msgRoot.attach(msgAlternative)
            msgText = MIMEText(
                'Check out our www.facebook.com/enchantedphotoboothoc/')
            msgAlternative.attach(msgText)

            print(e)
            photoPaths = e['PhotoPath']
            print(photoPaths)
            imgTags = ['<img src="cid:image{}">'.format(i) for i, x in enumerate(photoPaths)]

            with open(photoPaths, 'rb') as fp:
                msgImage = MIMEImage(fp.read())
                msgImage.add_header('Content-ID', '<image{}>'.format(0))
                msgRoot.attach(msgImage)
                print(photoPaths, "attached!")
                # break

            # We reference the image in the IMG SRC attribute by the ID we give it below
            msgText = MIMEText(
                'Thank you for using the Enchanted Mirror!<br><br>{}<br><br>Check out our www.facebook.com/enchantedphotoboothoc/<br><br>Sincerely,<br>Enchanted Mirror Team'.format('<br>'.join(imgTags)),
                'html')
            msgAlternative.attach(msgText)
            m = msgRoot.as_string()
            print(m)
            with open("content.txt",'w') as fp:
                fp.write(msgRoot.as_string())
            s.sendmail(strFrom, strTo, msgRoot.as_string())
            print('Email sent!')
            # break



