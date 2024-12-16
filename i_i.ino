#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>

uint8_t id;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial);

#define enroll A0
#define del 8
#define up A2
#define down A3
#define match A1
#define buzzer 9
#define resultsw 2
#define sw1 5
#define sw2 4
#define sw3 3
#define indVote 6
#define indFinger 7
#define records 25

int vote1, vote2, vote3;
int flag;

// Set the LCD address to 0x27 for a 16 chars and 2-line display
LiquidCrystal_I2C lcd(0x3F, 16, 2);

void setup() 
{
    delay(1000);

    pinMode(enroll, INPUT_PULLUP);
    pinMode(up, INPUT_PULLUP); 
    pinMode(down, INPUT_PULLUP); 
    pinMode(del, INPUT_PULLUP);
    pinMode(match, INPUT_PULLUP);
    pinMode(sw1, INPUT_PULLUP); 
    pinMode(sw2, INPUT_PULLUP);
    pinMode(sw3, INPUT_PULLUP);
    pinMode(resultsw, INPUT_PULLUP);
    pinMode(buzzer, OUTPUT);
    pinMode(indVote, OUTPUT);
    pinMode(indFinger, OUTPUT);

    lcd.begin();  // Initialize the LCD

    if (digitalRead(resultsw) == 0)
    {
        for (int i = 0; i < records; i++)
            EEPROM.write(i + 10, 0xff);
        EEPROM.write(0, 0);
        EEPROM.write(1, 0);
        EEPROM.write(2, 0);
        lcd.clear();
        lcd.print("System Reset");
        delay(1000);
    }

    lcd.clear();
    lcd.print("Voting Machine");
    lcd.setCursor(0, 1);
    lcd.print("by Finger Print");
    delay(2000);
    lcd.clear();
    lcd.print("Circuit Digest");
    lcd.setCursor(0, 1);
    lcd.print("Rifat");
    delay(2000);

    if (EEPROM.read(0) == 0xff)
        EEPROM.write(0, 0);
    if (EEPROM.read(1) == 0xff)
        EEPROM.write(1, 0);
    if (EEPROM.read(2) == 0xff)
        EEPROM.write(2, 0);

    Serial.begin(57600);

    lcd.clear();
    lcd.print("Finding Module");
    lcd.setCursor(0, 1);
    delay(1000);

    if (finger.verifyPassword())
    {
        lcd.clear();
        lcd.print("Found Module ");
        delay(1000);
    } 
    else 
    {
        lcd.clear();
        lcd.print("module not Found");
        lcd.setCursor(0, 1);
        lcd.print("Check Connections");
        while (1);
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cn1");
    lcd.setCursor(4, 0);
    lcd.print("Cn2");
    lcd.setCursor(8, 0);
    lcd.print("Cn3");

    lcd.setCursor(0, 1);
    vote1 = EEPROM.read(0);
    lcd.print(vote1);
    lcd.setCursor(6, 1);
    vote2 = EEPROM.read(1);
    lcd.print(vote2);
    lcd.setCursor(12, 1);
    vote3 = EEPROM.read(2);
    lcd.print(vote3);
    delay(2000);
}

void loop() 
{
    lcd.setCursor(0, 0);
    lcd.print("Press Match Key ");
    lcd.setCursor(0, 1);
    lcd.print("to start system");

    digitalWrite(indVote, LOW);
    digitalWrite(indFinger, LOW);

    // Check if the result button is pressed
    if (digitalRead(resultsw) == 0) {
        digitalWrite(buzzer, HIGH);
        delay(200);
        digitalWrite(buzzer, LOW);

        // Call the function to show the results
        showResults();
    }

    // Check if the match key is pressed to start voting
    if (digitalRead(match) == 0)
    {
        digitalWrite(buzzer, HIGH);
        delay(200);
        digitalWrite(buzzer, LOW);
        digitalWrite(indFinger, HIGH);

        for (int i = 0; i < 3; i++)
        {
            lcd.clear();
            lcd.print("Place Finger");
            delay(2000);
            int result = getFingerprintIDez();
            if (result >= 0)
            {
                flag = 0;
                for (int i = 0; i < records; i++)
                {
                    if (result == EEPROM.read(i + 10))
                    {
                        lcd.clear();
                        lcd.print("Authorised Voter");
                        lcd.setCursor(0, 1);
                        lcd.print("Please Wait....");
                        delay(1000);
                        Vote();
                        EEPROM.write(i + 10, 0xff); // Mark voter as having voted
                        flag = 1;
                        return;
                    }
                }

                if (flag == 0)
                {
                    lcd.clear();
                    lcd.print("Already Voted");
                    digitalWrite(buzzer, HIGH);
                    delay(5000);
                    digitalWrite(buzzer, LOW);
                    return;
                }
            }
        }
        lcd.clear();
    }
    checkKeys();
    delay(1000);
}

void checkKeys()
{
    if (digitalRead(enroll) == 0)
    {
        lcd.clear();
        lcd.print("Please Wait");
        delay(1000);
        while (digitalRead(enroll) == 0);
        Enroll();
    }
    else if (digitalRead(del) == 0)
    {
        lcd.clear();
        lcd.print("Please Wait");
        delay(1000);
        delet();
    }  
}

void Enroll()
{
    int count = 0;
    lcd.clear();
    lcd.print("Enter Finger ID:");

    while (1)
    {
        lcd.setCursor(0, 1);
        lcd.print(count);
        if (digitalRead(up) == 0)
        {
            count++;
            if (count > 25)
                count = 0;
            delay(500);
        }
        else if (digitalRead(down) == 0)
        {
            count--;
            if (count < 0)
                count = 25;
            delay(500);
        }
        else if (digitalRead(del) == 0)
        {
            id = count;
            getFingerprintEnroll();
            for (int i = 0; i < records; i++)
            {
                if (EEPROM.read(i + 10) == 0xff)
                {
                    EEPROM.write(i + 10, id);
                    break;
                }
            }
            return;
        }
        else if (digitalRead(enroll) == 0)
        {        
            return;
        }
    }
}

void delet()
{
    int count = 0;
    lcd.clear();
    lcd.print("Enter Finger ID");

    while (1)
    {
        lcd.setCursor(0, 1);
        lcd.print(count);
        if (digitalRead(up) == 0)
        {
            count++;
            if (count > 25)
                count = 0;
            delay(500);
        }
        else if (digitalRead(down) == 0)
        {
            count--;
            if (count < 0)
                count = 25;
            delay(500);
        }
        else if (digitalRead(del) == 0)
        {
            id = count;
            deleteFingerprint(id);
            for (int i = 0; i < records; i++)
            {
                if (EEPROM.read(i + 10) == id)
                {
                    EEPROM.write(i + 10, 0xff);
                    break;
                }
            }
            return;
        }
        else if (digitalRead(enroll) == 0)
        {        
            return;
        }
    }
}

void showResults() {
    // Clear the LCD for a clean display
    lcd.clear();
    lcd.print("Results:");

    // Read vote counts from EEPROM
    vote1 = EEPROM.read(0);
    vote2 = EEPROM.read(1);
    vote3 = EEPROM.read(2);

    // Display vote counts for each candidate
    lcd.setCursor(0, 1);
    lcd.print("C1:");
    lcd.print(vote1);
    lcd.setCursor(6, 1);
    lcd.print("C2:");
    lcd.print(vote2);
    lcd.setCursor(12, 1);
    lcd.print("C3:");
    lcd.print(vote3);

    // Wait for some time to allow the user to view the results
    delay(5000); // Adjust the delay as per your requirement
}

uint8_t getFingerprintEnroll() 
{
    int p = -1;
    lcd.clear();
    lcd.print("finger ID:");
    lcd.print(id);
    lcd.setCursor(0, 1);
    lcd.print("Place Finger");
    delay(2000);
    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
        switch (p)
        {
            case FINGERPRINT_OK:
                lcd.clear();
                lcd.print("Image taken");
                break;
            case FINGERPRINT_NOFINGER:
                lcd.clear();
                lcd.print("No finger detected");
                break;
            case FINGERPRINT_PACKETRECIEVEERR:
                lcd.clear();
                lcd.print("Communication error");
                break;
            case FINGERPRINT_IMAGEFAIL:
                lcd.clear();
                lcd.print("Imaging error");
                break;
            default:
                lcd.clear();
                lcd.print("Unknown error");
                break;
        }
    }

    p = finger.image2Tz(1);
    switch (p)
    {
        case FINGERPRINT_OK:
            lcd.clear();
            lcd.print("Image converted");
            break;
        case FINGERPRINT_IMAGEMESS:
            lcd.clear();
            lcd.print("Image too messy");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            lcd.clear();
            lcd.print("Communication error");
            return p;
        case FINGERPRINT_FEATUREFAIL:
            lcd.clear();
            lcd.print("Could not find fingerprint features");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            lcd.clear();
            lcd.print("Invalid fingerprint image");
            return p;
        default:
            lcd.clear();
            lcd.print("Unknown error");
            return p;
    }

    lcd.clear();
    lcd.print("Remove finger");
    delay(2000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER)
    {
        p = finger.getImage();
    }

    lcd.clear();
    lcd.print("Place same finger");
    delay(2000);
    p = -1;
    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
        switch (p)
        {
            case FINGERPRINT_OK:
                lcd.clear();
                lcd.print("Image taken");
                break;
            case FINGERPRINT_NOFINGER:
                lcd.clear();
                lcd.print("No finger detected");
                break;
            case FINGERPRINT_PACKETRECIEVEERR:
                lcd.clear();
                lcd.print("Communication error");
                break;
            case FINGERPRINT_IMAGEFAIL:
                lcd.clear();
                lcd.print("Imaging error");
                break;
            default:
                lcd.clear();
                lcd.print("Unknown error");
                break;
        }
    }

    p = finger.image2Tz(2);
    switch (p)
    {
        case FINGERPRINT_OK:
            lcd.clear();
            lcd.print("Image converted");
            break;
        case FINGERPRINT_IMAGEMESS:
            lcd.clear();
            lcd.print("Image too messy");
            return p;
        case FINGERPRINT_PACKETRECIEVEERR:
            lcd.clear();
            lcd.print("Communication error");
            return p;
        case FINGERPRINT_FEATUREFAIL:
            lcd.clear();
            lcd.print("Could not find fingerprint features");
            return p;
        case FINGERPRINT_INVALIDIMAGE:
            lcd.clear();
            lcd.print("Invalid fingerprint image");
            return p;
        default:
            lcd.clear();
            lcd.print("Unknown error");
            return p;
    }

    p = finger.createModel();
    if (p == FINGERPRINT_OK)
    {
        lcd.clear();
        lcd.print("Prints matched");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        lcd.clear();
        lcd.print("Communication error");
        return p;
    }
    else if (p == FINGERPRINT_ENROLLMISMATCH)
    {
        lcd.clear();
        lcd.print("Fingerprints did not match");
        return p;
    }
    else
    {
        lcd.clear();
        lcd.print("Unknown error");
        return p;
    }

    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK)
    {
        lcd.clear();
        lcd.print("Stored!");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        lcd.clear();
        lcd.print("Communication error");
        return p;
    }
    else if (p == FINGERPRINT_BADLOCATION)
    {
        lcd.clear();
        lcd.print("Could not store in that location");
        return p;
    }
    else if (p == FINGERPRINT_FLASHERR)
    {
        lcd.clear();
        lcd.print("Error writing to flash");
        return p;
    }
    else
    {
        lcd.clear();
        lcd.print("Unknown error");
        return p;
    }
    delay(2000);
}

int getFingerprintIDez()
{
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK)
        return -1;

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK)
        return -1;

    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK)
        return -1;

    return finger.fingerID;
}

uint8_t deleteFingerprint(uint8_t id)
{
    uint8_t p = -1;
    p = finger.deleteModel(id);

    if (p == FINGERPRINT_OK)
    {
        lcd.clear();
        lcd.print("Deleted!");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        lcd.clear();
        lcd.print("Communication error");
        return p;
    }
    else if (p == FINGERPRINT_BADLOCATION)
    {
        lcd.clear();
        lcd.print("Could not delete in that location");
        return p;
    }
    else if (p == FINGERPRINT_FLASHERR)
    {
        lcd.clear();
        lcd.print("Error writing to flash");
        return p;
    }
    else
    {
        lcd.clear();
        lcd.print("Unknown error");
        return p;
    }
    delay(2000);
}

void Vote()
{
    digitalWrite(indVote, HIGH);
    while (1)
    {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Vote For:");
        lcd.setCursor(0, 1);
        lcd.print("1   2   3");
        delay(1000);

        if (digitalRead(sw1) == 0)
        {
            vote1++;
            EEPROM.write(0, vote1);
            lcd.clear();
            lcd.print("You Voted For:");
            lcd.setCursor(0, 1);
            lcd.print("Candidate 1");
            delay(2000);
            return;
        }
        else if (digitalRead(sw2) == 0)
        {
            vote2++;
            EEPROM.write(1, vote2);
            lcd.clear();
            lcd.print("You Voted For:");
            lcd.setCursor(0, 1);
            lcd.print("Candidate 2");
            delay(2000);
            return;
        }
        else if (digitalRead(sw3) == 0)
        {
            vote3++;
            EEPROM.write(2, vote3);
            lcd.clear();
            lcd.print("You Voted For:");
            lcd.setCursor(0, 1);
            lcd.print("Candidate 3");
            delay(2000);
            return;
        }
 }
}
