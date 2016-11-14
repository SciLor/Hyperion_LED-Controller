#ifndef WrapperWebconfig_h
#define WrapperWebconfig_h
#include "BaseHeader.h"

#include <ESP8266WebServer.h>
#include "WrapperFastLed.h"
#include <LinkedList.h>

class SelectEntryBase {
  public:
    SelectEntryBase() {};
    SelectEntryBase(String selectedValue, String text, boolean selected) {
      _selectedValue = selectedValue;
      _text = text;
      _selected = selected;
    };
    
    String getSelectedValue(void) { return _selectedValue; };
    String getText(void) { return _text; };
    boolean isSelected(void) { return _selected; };
    
    void setSelected(boolean selected) { _selected = selected; };
  private:    
    String _selectedValue;
    String _text;
    boolean _selected; 
};

template <class T>
class SelectEntry : public SelectEntryBase {
  public:
    SelectEntry() { };
    SelectEntry(String selectedValue, String text, boolean selected, T value) : SelectEntryBase(selectedValue, text, selected) {
      _value = value;
    }
    T getValue(void) { return _value; };
  private:
    T _value;
};
  
class WrapperWebconfig {  
  public:
    void
      begin(),
      handle(void);
  private:
    void
      handleNotFound(void),
      handleRoot(void),
      changeConfig(void),
      parseBytes(const char* str, char sep, byte* bytes, int maxBytes, int base);
      
    String
      escape(String text),
      escape(char* text),
      escape(uint8_t text),
      escape(uint16_t text),
      ipToString(ConfigIP ip),
      
      htmlTemplate(String title, String content),
      groupTemplate(String title, String body),
      entryTemplate(String title, String id, String content),
      textTemplate(String title, String id, String text, String placeholder, int maxLen),
      selectTemplate(String title, String id, LinkedList<SelectEntryBase*>* entries),
      config(void);

    void
      initHelperVars(void),
      clearHelperVars(void),
      clearLinkedList(LinkedList<SelectEntryBase*>* target),
      getIdleModes(uint8_t active, LinkedList<SelectEntryBase*>* target);

    template<typename T>
    T getSelectedEntry(String selectedEntryValue, LinkedList<SelectEntryBase*>* target);
    
    LinkedList<SelectEntryBase*>* _idleModes;
    
    ESP8266WebServer _server = ESP8266WebServer(80);
};

#endif
