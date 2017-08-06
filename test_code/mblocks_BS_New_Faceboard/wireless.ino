void sendMessage() ; // Prototype

void sendMessage() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  bool error = mesh.sendBroadcast(msg + " myFreeMemory: " + String(ESP.getFreeHeap()));

  if (calc_delay) {
    SimpleList<uint32_t>::iterator node = nodes.begin();
    while (node != nodes.end()) {
      mesh.startDelayMeas(*node);
      node++;
    }
    calc_delay = false;
  }
  
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));  // between 1 and 5 seconds
}


void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  if(String(msg.c_str()) == "green"){turn_color(green); Serial.println("HEY BRO!!");}
  else if(String(msg.c_str()) == "red"){turn_color(red); Serial.println("HEY BRO!!");}
  else if(String(msg.c_str()) == "purple"){turn_color(purple); Serial.println("HEY BRO!!");}
}

void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections %s\n", mesh.subConnectionJson().c_str());

  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}
