import java.io.*; 
import java.net.*;

/**
 * Example Java client program with text user interface for demonstrating the Telemetry Interface introduced in NoLimits 2.2.0.0
 *
 * Client Demo Version 0.3
 * Copyright 2015 Ole Lange
 *
 * The protocol is a binary message based protocol using TCP. Each message will be answered by the server with a specific message in the same order as messages were sent by the client.
 * Each message has the same basic binary format. All multi-byte values are in network-byte-order (big-endian)!!
 * The minimum message size is 10 bytes.
 *
 * Default TCP port of NoLimits 2 telemetry server is: 15151
 * Telemetry server needs to be enabled by starting NoLimits2 with command line parameter '--telemetry'
 * Port can be changed with command line parameter '--telemetryport=<port>'
 *
 * E.g. for starting the server on port 15152, start with commandline: '"c:\Program files\NoLimits 2\64bit\NoLimits2app.exe" --telemetry --telemetryport=15152'
 * 
 * Message Format Structure:
 * ByteOffset  Type      Size      Meaning 
 * ====================================================================
 * 0           uchar8    1         Message Start (magic number, value = 'N')
 * 1           ushort16  2         MessageType (see message enum table)
 * 3           uint32    4         Request ID (can be freely assigned by client)
 * 7           ushort16  2         DataSize (depends on message type)
 * 9           varying   DataSize  Depends on message type
 * 9+DataSize  uchar8    1         Message End (magic number, value = 'L')
 */
public class NL2TelemetryClient
{
  /**
   * Message Enum
   * Type: Request
   * Can be send by the client to keep connetion alive. No other purpose. Returned message by server is MSG_OK
   * DataSize = 0
   */
  private static final int N_MSG_IDLE = 0;         

  /**
   * Message Enum
   * Type: Reply
   * Typical answer from server for messages that were successfully processed and do not require a specific returned answer
   * DataSize = 0
   */
  private static final int N_MSG_OK = 1;      
 
  /**
   * Message Enum
   * Type: Reply   
   * Will be send by the server in case of an error. The data component contains an UTF-8 encoded error message
   * DataSize = number of bytes of UTF8 encoded string
   *    UTF8 string
   */
  private static final int N_MSG_ERROR = 2;
  
  /**
   * Message Enum
   * Type: Request   
   * Can be used by the client to request the application version. The server will reply with MSG_VERSION
   * DataSize = 0
   */
  private static final int N_MSG_GET_VERSION = 3;
  
  /**
   * Message Enum
   * Type: Reply   
   * Will be send by the server as an answer to MSG_GET_VERSION
   * DataSize = 4 
   *    4 Bytes (major to minor version numbers e.g. 2, 2, 0, 0 for '2.2.0.0')
   */
  private static final int N_MSG_VERSION = 4;
  
  /**
   * Message Enum
   * Type: Request   
   * Can be used by the client to request common telemetry data. The server will reply with MSG_TELEMETRY
   * DataSize = 0
   */
  private static final int N_MSG_GET_TELEMETRY = 5;
  
  /**
   * Message Enum
   * Type: Reply   
   * Will be send by the server as an anwser to MSG_GET_TELEMETRY
   * DataSize = 76 
   *    int32 (state flags)
   *      bit0 -> in play mode
   *      bit1 -> braking
   *      bit2-31 -> reserved
   *    int32 (current rendered frame number) -> can be used to detect if telemetry data is new
   *    int32 (view mode)
   *    int32 (current coaster)
   *    int32 (coaster style id)
   *    int32 (current train)
   *    int32 (current car)
   *    int32 (current seat)
   *    float32 (speed)
   *    float32 (Position x)
   *    float32 (Position y)
   *    float32 (Position z)
   *    float32 (Rotation quaternion x)
   *    float32 (Rotation quaternion y)
   *    float32 (Rotation quaternion z)
   *    float32 (Rotation quaternion w)
   *    float32 (G-Force x)
   *    float32 (G-Force y)
   *    float32 (G-Force z)   
   */
  private static final int N_MSG_TELEMETRY = 6;
  
  /**
   * Message Enum
   * Type: Request   
   * Can be used by the client to request the number of coasters. The server will reply with MSG_INT_VALUE
   * DataSize = 0
   */
  private static final int N_MSG_GET_COASTER_COUNT = 7;
  
  /**
   * Message Enum
   * Type: Reply   
   * Will be send by the server as an answer to messages requesting various numbers
   * DataSize = 4
   *    int32 (meaning depends on requested information)
   */
  private static final int N_MSG_INT_VALUE = 8;
  
  /**
   * Message Enum
   * Type: Request   
   * Can be used by the client to request the name of a specific coaster. The server will reply with MSG_STRING
   * DataSize = 4
   *    int32 (coaster index 0..N-1), use MSG_GET_COASTER_COUNT to query the number of available coasters
   */
  private static final int N_MSG_GET_COASTER_NAME = 9;
  
  /**
   * Message Enum 
   * Type: Reply
   * Will be send by the server as an answer to messages requesting various strings
   * DataSize = length of UTF8 encoded string
   *    UTF8 string
   */
  private static final int N_MSG_STRING = 10;
  
  /**
   * Message Enum
   * Type: Request
   * Can be used by the client to request the current coaster and nearest station indices. The server will reply with MSG_INT_VALUE_PAIR
   * DataSize = 0
   */
  private static final int N_MSG_GET_CURRENT_COASTER_AND_NEAREST_STATION = 11;
  
  /**
   * Message Enum
   * Type: Reply
   * Will be send by the server as an answer to messages requesting various value pairs
   * DataSize = 8
   *    int32 (first value)
   *    int32 (second value)
   */
  private static final int N_MSG_INT_VALUE_PAIR = 12;
  
  /**
   * Message Enum
   * Type: Request
   * Can be used by the client to set the emergency stop. The server will reply with MSG_OK
   * DataSize = 5
   *    int32 (coaster index)
   *    uchar8 (1 = on, 0 = off)
   */
  private static final int N_MSG_SET_EMERGENCY_STOP = 13;
  
  /**
   * Message Enum 
   * Type: Request
   * Can be used by the client to request the state of a specific station. The server will reply with MSG_STATION_STATE
   * DataSize = 8
   *    int32 (coaster index)
   *    int32 (station index (from MSG_GET_CURRENT_COASTER_AND_NEAREST_STATION))
   */
  private static final int N_MSG_GET_STATION_STATE = 14; //size=8 (int32=coaster index, int32=station index)
  
  /**
   * Message Enum
   * Type: Reply
   * Will be send by server as an answer to a MSG_GET_STATION_STATE messge
   * DataSize = 4
   *    int32 (flags)
   *      bit0 -> E-Stop On/Off
   *      bit1 -> Manual Dispatch On/Off
   *      bit2 -> Can Dispatch
   *      bit3 -> Can Close Gates
   *      bit4 -> Can Open Gates
   *      bit5 -> Can Close Harness 
   *      bit6 -> Can Open Harness
   *      bit7 -> Can Raise Platform
   *      bit8 -> Can Lower Platform
   *      bit9 -> Can Lock Flyer Car
   *      bit10 -> Can Unlock Flyer Car
   *      bit11-31 -> reserved
   */
  private static final int N_MSG_STATION_STATE = 15;

  /**
   * Message Enum
   * Type: Request
   * Can be used by the client to switch between manual and automatic station mode
   * DataSize = 9
   *    int32 (coaster index)
   *    int32 (station index)
   *    uchar8 (1 = on, 0 = off)
   */  
  private static final int N_MSG_SET_MANUAL_MODE = 16;
 
  /**
   * Message Enum
   * Type: Request
   * Can be used by the client to dispatch a train in manual mode
   * DataSize = 8
   *    int32 (coaster index)
   *    int32 (station index)
   */
  private static final int N_MSG_DISPATCH = 17; // size=8 (int32 = coaster index, int32 = station index)
  
  /**
   * Message Enum
   * Type: Request
   * Can be used by the client to change gates in manual mode
   * DataSize = 9
   *    int32 (coaster index)
   *    int32 (station index)
   *    uchar8 (1 = on, 0 = off)
   */ 
  private static final int N_MSG_SET_GATES = 18; // size=9 (int32= coaster index, int32= station index, byte= boolean (false=closed/true=open))
  
  /**
   * Message Enum
   * Type: Request
   * Can be used by the client to change harness in manual mode
   * DataSize = 9
   *    int32 (coaster index)
   *    int32 (station index)
   *    uchar8 (1 = on, 0 = off)
   */
  private static final int N_MSG_SET_HARNESS = 19; // size=9 (int32= coaster index, int32= station index, byte= boolean (false=closed/true=open))
  
  /**
   * Message Enum
   * Type: Request
   * Can be used by the client to lower/raise platform in manual mode
   * DataSize = 9
   *    int32 (coaster index)
   *    int32 (station index)
   *    uchar8 (1 = on, 0 = off)
   */
  private static final int N_MSG_SET_PLATFORM = 20; // size=9 (int32= coaster index, int32= station index, byte= boolean (false=raised/true=lowered))
  
  /**
   * Message Enum
   * Type: Request
   * Can be used by the client to lock/unlock flyer car in manual mode
   * DataSize = 9
   *    int32 (coaster index)
   *    int32 (station index)
   *    uchar8 (1 = on, 0 = off)
   */
  private static final int N_MSG_SET_FLYER_CAR = 21; // size=9 (int32= coaster index, int32= station index, byte= boolean (false=locked/true=unlocked))

  /**
   * Start of extra size data within message
   */
  private static final int c_nExtraSizeOffset = 9;
  
  /**
   * The request ID can be freely assigned by the client, we simply use an increasing counter.
   * The server will reply to request messages using the same id.
   * The request ID can be used to identify matching replys from the server to requests from the client.
   */
  private static int s_nRequestId;
  
  public static void main(String args[])
  {
    if (args.length < 1 || args.length > 2)
    {
      System.err.println("Invalid arguments");
      printSyntax();
      return;
    }
    
    String server = args[0];
    String portstr = "15151";
    
    if (args.length == 2)
    {
      portstr = args[1];
    }
    
    int port = Integer.parseInt(portstr);
    
    BufferedReader inFromUser = new BufferedReader( new InputStreamReader(System.in));
    
    System.out.println("Trying to connect to " + server + ":" + port + " ...");
    
    try
    {
      Socket clientSocket = new Socket(server, port);
      
      clientSocket.setKeepAlive(true);
      
      System.out.println("...connected!");
      
      System.out.println("Enter 'help' for list of available commands");
      
      DataOutputStream outToServer = new DataOutputStream(clientSocket.getOutputStream());
      DataInputStream inFromServer = new DataInputStream(clientSocket.getInputStream());
      
      
      while (!clientSocket.isClosed())
      {
        System.out.print("Enter command: ");
        String sentence = inFromUser.readLine();
        if (sentence == null || sentence.equals("quit") || sentence.equals("exit"))
        {
          break;
        }
        
        byte []bytes = decodeCommand(sentence);
        if (bytes != null)
        {        
          long nTime1 = System.currentTimeMillis();
        
          outToServer.write(bytes, 0, bytes.length);
        
          bytes = readMessage(inFromServer);
        
          long nTime2 = System.currentTimeMillis();
          
          decodeMessage(bytes);
        
          System.out.println("Latency: " + (nTime2 - nTime1) + "ms");
        }
      }
      clientSocket.close();
    }
    catch (Exception e)
    {
      e.printStackTrace(System.err);
    }
  }  
  
  /**
   * Encode n as two bytes (ushort16/sshort16) in network byte order (big-endian)
   */
  private static void encodeUShort16(byte[] bytes, int offset, int n)
  {
    bytes[offset] = (byte)((n >> 8) & 0xFF);
    bytes[offset+1] = (byte)(n & 0xFF);
  }

  /**
   * Encode n as four bytes (uint32/int32) in network byte order (big-endian)
   */  
  private static void encodeInt32(byte[] bytes, int offset, int n)
  {
    bytes[offset] = (byte)((n >> 24) & 0xFF);    
    bytes[offset+1] = (byte)((n >> 16) & 0xFF);    
    bytes[offset+2] = (byte)((n >> 8) & 0xFF);
    bytes[offset+3] = (byte)(n & 0xFF);
  }  
  
  /**
   * Encode b as one byte
   */  
  private static void encodeBoolean(byte[] bytes, int offset, boolean b)
  {
    bytes[offset] = (byte)(b ? 1 : 0); 
  }
  
  /**
   * Decode one byte as boolean
   */  
  private static boolean decodeBoolean(byte[] bytes, int offset)
  {
    return bytes[offset] != 0; 
  }
  
  /**
   * Decode two bytes in network byte order (big-endian) as ushort16
   */  
  private static int decodeUShort16(byte b1, byte b2)
  {
    int n1 = (((int)b1) & 0xFF) << 8;
    int n2 = ((int)b2) & 0xFF;    
    return n1 | n2; 
  }

  /**
   * Decode four bytes in network byte order (big-endian) as int32
   */    
  private static int decodeInt32(byte[] bytes, int offset)
  {
    int n1 = (((int)bytes[offset]) & 0xFF) << 24;
    int n2 = (((int)bytes[offset+1]) & 0xFF) << 16;    
    int n3 = (((int)bytes[offset+2]) & 0xFF) << 8;  
    int n4 = ((int)bytes[offset+3]) & 0xFF;    
    return n1 | n2 | n3 | n4;
  }

  /**
   * Decode four bytes in network byte order (big-endian) as float32
   */      
  private static float decodeFloat(byte[] bytes, int offset)
  {
    return Float.intBitsToFloat(decodeInt32(bytes, offset)); 
  }

  /**
   * Decode len bytes as UTF8-string
   */        
  private static String decodeString(byte[] bytes, int offset, int len)
  {
    String str;
    
    try
    {
      str = new String(bytes, offset, len, "UTF-8");
    }
    catch (java.io.UnsupportedEncodingException e)
    {
      str = null;
    }
    return str;
  }
  
  /**
   * Create a message with DataSize=0
   */  
  private static byte[] createSimpleMessage(int requestId, int msg)
  {
    byte[] bytes = new byte[10];
    bytes[0] = (byte)'N';
    encodeUShort16(bytes, 1, msg);
    encodeInt32(bytes, 3, requestId);
    encodeUShort16(bytes, 7, 0);
    bytes[9] = (byte)'L';
    return bytes;
  }
  
  private static byte[] createComplexMessage(int requestId, int msg, int extraSize)
  {
    if (extraSize < 0 || extraSize > 65535) return null;
    byte[] bytes = new byte[10 + extraSize];
    bytes[0] = (byte)'N';
    encodeUShort16(bytes, 1, msg);
    encodeInt32(bytes, 3, requestId);
    encodeUShort16(bytes, 7, extraSize);
    bytes[9 + extraSize] = (byte)'L';
    return bytes;
  }

  /**
   * Show information about required syntax of program
   */  
  private static void printSyntax()
  {
    System.out.println("Syntax: NL2TelemetryClient server [port]");
  }
 
  /**
   * Parse command and create correspondig message
   */  
  private static byte[] decodeCommand(String in)
  {
    if (in.equals("i") || in.equals("idle"))
    {
      return createSimpleMessage(s_nRequestId++, N_MSG_IDLE);
    }
    else if (in.equals("gv") || in.equals("getversion"))
    {
      return createSimpleMessage(s_nRequestId++, N_MSG_GET_VERSION); 
    }
    else if (in.equals("gt") || in.equals("gettelemetry"))
    {
      return createSimpleMessage(s_nRequestId++, N_MSG_GET_TELEMETRY); 
    }
    else if (in.equals("gcc") || in.equals("getcoastercount"))
    {
      return createSimpleMessage(s_nRequestId++, N_MSG_GET_COASTER_COUNT);
    }
    else if (in.equals("gcn") || in.equals("getcoastername"))
    {
      int nCoasterIndex = 0; // first coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_GET_COASTER_NAME, 4);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      return bytes;
    }
    else if (in.equals("gccns") || in.equals("getcurrentcoasterandneareststation"))
    {
      return createSimpleMessage(s_nRequestId++, N_MSG_GET_CURRENT_COASTER_AND_NEAREST_STATION);
    }
    else if (in.equals("seon") || in.equals("setemergencystopon"))
    {
      int nCoasterIndex = 0; // first coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_SET_EMERGENCY_STOP, 5);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      encodeBoolean(bytes, c_nExtraSizeOffset+4, true);
      return bytes;      
    }
    else if (in.equals("seoff") || in.equals("setemergencystopoff"))
    {
      int nCoasterIndex = 0; // first coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_SET_EMERGENCY_STOP, 5);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      encodeBoolean(bytes, c_nExtraSizeOffset+4, false);
      return bytes;      
    }
    else if (in.equals("gss") || in.equals("getstationstate"))
    {
      int nCoasterIndex = 0; // first coaster
      int nStationIndex = 0; // first station of coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_GET_STATION_STATE, 8);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      encodeInt32(bytes, c_nExtraSizeOffset+4, nStationIndex);
      return bytes;
    } 
    else if (in.equals("setsmmon") || in.equals("setstationmanualmodeon"))
    {
      int nCoasterIndex = 0; // first coaster
      int nStationIndex = 0; // first station of coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_SET_MANUAL_MODE, 9);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      encodeInt32(bytes, c_nExtraSizeOffset+4, nStationIndex);
      encodeBoolean(bytes, c_nExtraSizeOffset+8, true);
      return bytes;      
    }
    else if (in.equals("setsmmoff") || in.equals("setstationmanualmodeoff"))
    {
      int nCoasterIndex = 0; // first coaster
      int nStationIndex = 0; // first station of coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_SET_MANUAL_MODE, 9);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      encodeInt32(bytes, c_nExtraSizeOffset+4, nStationIndex);
      encodeBoolean(bytes, c_nExtraSizeOffset+8, false);
      return bytes;      
    }
    else if (in.equals("d") || in.equals("dispatch"))
    {
      int nCoasterIndex = 0; // first coaster
      int nStationIndex = 0; // first station of coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_DISPATCH, 8);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      encodeInt32(bytes, c_nExtraSizeOffset+4, nStationIndex);
      return bytes;      
    }
    else if (in.equals("sgc") || in.equals("stationgatesclose"))
    {
      int nCoasterIndex = 0; // first coaster
      int nStationIndex = 0; // first station of coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_SET_GATES, 9);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      encodeInt32(bytes, c_nExtraSizeOffset+4, nStationIndex);
      encodeBoolean(bytes, c_nExtraSizeOffset+8, false);
      return bytes;      
    }
    else if (in.equals("sgo") || in.equals("stationgatesopen"))
    {
      int nCoasterIndex = 0; // first coaster
      int nStationIndex = 0; // first station of coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_SET_GATES, 9);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      encodeInt32(bytes, c_nExtraSizeOffset+4, nStationIndex);
      encodeBoolean(bytes, c_nExtraSizeOffset+8, true);
      return bytes;      
    }
    else if (in.equals("shc") || in.equals("stationharnessclose"))
    {
      int nCoasterIndex = 0; // first coaster
      int nStationIndex = 0; // first station of coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_SET_HARNESS, 9);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      encodeInt32(bytes, c_nExtraSizeOffset+4, nStationIndex);
      encodeBoolean(bytes, c_nExtraSizeOffset+8, false);
      return bytes;      
    }
    else if (in.equals("sho") || in.equals("stationharnessopen"))
    {
      int nCoasterIndex = 0; // first coaster
      int nStationIndex = 0; // first station of coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_SET_HARNESS, 9);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      encodeInt32(bytes, c_nExtraSizeOffset+4, nStationIndex);
      encodeBoolean(bytes, c_nExtraSizeOffset+8, true);
      return bytes;      
    }  
    else if (in.equals("spr") || in.equals("stationplatformraise"))
    {
      int nCoasterIndex = 0; // first coaster
      int nStationIndex = 0; // first station of coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_SET_PLATFORM, 9);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      encodeInt32(bytes, c_nExtraSizeOffset+4, nStationIndex);
      encodeBoolean(bytes, c_nExtraSizeOffset+8, false);
      return bytes;      
    }
    else if (in.equals("spl") || in.equals("stationplatformlower"))
    {
      int nCoasterIndex = 0; // first coaster
      int nStationIndex = 0; // first station of coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_SET_PLATFORM, 9);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      encodeInt32(bytes, c_nExtraSizeOffset+4, nStationIndex);
      encodeBoolean(bytes, c_nExtraSizeOffset+8, true);
      return bytes;      
    }
    else if (in.equals("sfl") || in.equals("stationflyercarlock"))
    {
      int nCoasterIndex = 0; // first coaster
      int nStationIndex = 0; // first station of coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_SET_FLYER_CAR, 9);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      encodeInt32(bytes, c_nExtraSizeOffset+4, nStationIndex);
      encodeBoolean(bytes, c_nExtraSizeOffset+8, false);
      return bytes;      
    }
    else if (in.equals("sfu") || in.equals("stationflyercarunlock"))
    {
      int nCoasterIndex = 0; // first coaster
      int nStationIndex = 0; // first station of coaster
      byte[] bytes = createComplexMessage(s_nRequestId++, N_MSG_SET_FLYER_CAR, 9);
      encodeInt32(bytes, c_nExtraSizeOffset, nCoasterIndex);
      encodeInt32(bytes, c_nExtraSizeOffset+4, nStationIndex);
      encodeBoolean(bytes, c_nExtraSizeOffset+8, true);
      return bytes;      
    }
    else if (!in.equals("help"))
    {
      System.err.print("Invalid command '" + in + "'\n");
    }
    printValidCommands();
    return null;
  }
  
  /**
   * Show available command list
   */  
  private static void printValidCommands()
  {
    System.out.println("Valid commands: i (idle) - Send idle message");
    System.out.println("                help - Show available commands");
    System.out.println("                quit/exit - Leave client program");
    System.out.println("                gv (getversion) - Query server version");
    System.out.println("                gt (gettelemetry) - Query telemetry data");
    System.out.println("                gcc (getcoastercount) - Query number of coasters");
    System.out.println("                gcn (getcoastername) - Query name of first coaster");
    System.out.println("                gccns (getcurrentcoasterandneareststation) - Query current coaster index and nearest station");
    System.out.println("                seon (setemergencystopon) - Enable e-stop on first coaster");
    System.out.println("                seoff (setemergencystopoff) - Disable e-stop on first coaster");
    System.out.println("                gss (getstationstate) - Query state of first coaster's first station");
    System.out.println("                setsmmon (setstationmanualmodeon) - Enable manual mode of first coaster's first station");
    System.out.println("                setsmmoff (setstationmanualmodeoff) - Disable manual mode of first coaster's first station");
    System.out.println("                d (dispatch) - Dispatch train in first coaster's first station");
    System.out.println("                sgc (stationgatesclose) - Close gates in first coaster's first station");
    System.out.println("                sgo (stationgatesopen) - Open gates in first coaster's first station");
    System.out.println("                shc (stationharnessclose) - Close harnesses in first coaster's first station");
    System.out.println("                sho (stationharnessopen) - Open harnesses in first coaster's first station");
    System.out.println("                spr (stationplatformraise) - Raise platform in first coaster's first station");
    System.out.println("                spl (stationplatformlower) - Lower platform in first coaster's first station");
    System.out.println("                sfl (stationflyercarlock) - Lock flyer car in first coaster's first station");
    System.out.println("                sfu (stationflyercarunlock) - Unlock flyer car in first coaster's first station");    
  }
  
  /**
   * Receive a message from server
   */  
  private static byte[] readMessage(DataInputStream input) throws java.io.IOException, java.lang.InterruptedException, java.lang.Exception
  {
    int prefix = input.read();
    if (prefix != (int)'N')
    {
      if (prefix != -1)
      {
        throw new Exception("Invalid message received");
      }
      else
      {
        throw new Exception("No data from server");
      }
    }
    
    int b1 = input.read();
    if (b1 == -1)
    {
      throw new Exception("No data from server");
    }
    int b2 = input.read();
    if (b2 == -1)
    {
      throw new Exception("No data from server");
    }
    int b3 = input.read();
    if (b3 == -1)
    {
      throw new Exception("No data from server");
    }
    int b4 = input.read();
    if (b4 == -1)
    {
      throw new Exception("No data from server");
    }  
    
    int b5 = input.read();
    if (b5 == -1)
    {
      throw new Exception("No data from server");
    }
    
    int b6 = input.read();
    if (b6 == -1)
    {
      throw new Exception("No data from server");
    }    

    int b7 = input.read();
    if (b7 == -1)
    {
      throw new Exception("No data from server");
    }    
    
    int b8 = input.read();
    if (b8 == -1)
    {
      throw new Exception("No data from server");
    }    

    int extraSize = decodeUShort16((byte)b7, (byte)b8);    
    
    byte[] bytes = new byte[10 + extraSize];

    bytes[0] = (byte)prefix;
    bytes[1] = (byte)b1;    
    bytes[2] = (byte)b2;
    bytes[3] = (byte)b3;
    bytes[4] = (byte)b4;
    bytes[5] = (byte)b5;    
    bytes[6] = (byte)b6;
    bytes[7] = (byte)b7;
    bytes[8] = (byte)b8;    
    
    for (int i=0; i<extraSize; ++i)
    {
      int b = input.read();
      if (b == -1)
      {
        throw new Exception("No data from server");
      }      
      bytes[9+i] = (byte)b; 
    }
    
    int postfix = input.read();
    if (postfix != (int)'L')
    {
      if (postfix != -1)
      {
        throw new Exception("Invalid message received");
      }
      else
      {
        throw new Exception("No data from server");
      }
    }    
    
    bytes[9+extraSize] = (byte)postfix;
    
    return bytes;
  }
  
  /**
   * Decode a received message
   */   
  private static void decodeMessage(byte[] bytes)
  {
    int len = bytes.length;
    if (len >= 10)
    {
      int msg = decodeUShort16(bytes[1], bytes[2]);
      int requestId = decodeInt32(bytes, 3);
      int size = decodeUShort16(bytes[7], bytes[8]);
      if (size+10 == len)
      {
        System.out.print("Server replied to request " + requestId + ": ");
        switch (msg)
        {
        case N_MSG_IDLE:
          System.out.println("Idle");
          break;
        case N_MSG_OK:
          System.out.println("Ok");
          break;
        case N_MSG_ERROR:
          {
            System.out.println("Error: " + decodeString(bytes, c_nExtraSizeOffset, size));
          }
          break;
        case N_MSG_STRING:
          {
            System.out.println("String: " + decodeString(bytes, c_nExtraSizeOffset, size));
          }
          break;
        case N_MSG_VERSION:
          if (size == 4)
          {
          	System.out.println("Version: " + bytes[c_nExtraSizeOffset] + "." + bytes[c_nExtraSizeOffset+1] + "." + bytes[c_nExtraSizeOffset+2] + "." + bytes[c_nExtraSizeOffset+3]);
          }
          break;
        case N_MSG_TELEMETRY:
          if (size == 76)
          {
            int state = decodeInt32(bytes, c_nExtraSizeOffset);
            
            int frameNo = decodeInt32(bytes, c_nExtraSizeOffset+4);
            
            boolean inPlay = (state & 1) != 0;
            boolean onboard = (state & 2) != 0;
            boolean paused = (state & 4) != 0;
            
            int viewMode = decodeInt32(bytes, c_nExtraSizeOffset+8);
            int coasterIndex = decodeInt32(bytes, c_nExtraSizeOffset+12);
            int coasterStyleId = decodeInt32(bytes, c_nExtraSizeOffset+16);
            int currentTrain = decodeInt32(bytes, c_nExtraSizeOffset+20);
            int currentCar = decodeInt32(bytes, c_nExtraSizeOffset+24);
            int currentSeat = decodeInt32(bytes, c_nExtraSizeOffset+28);
            float speed = decodeFloat(bytes, c_nExtraSizeOffset+32);
            
            Quaternion quat = new Quaternion();
            
            float posx = decodeFloat(bytes, c_nExtraSizeOffset+36);
            float posy = decodeFloat(bytes, c_nExtraSizeOffset+40);
            float posz = decodeFloat(bytes, c_nExtraSizeOffset+44);
            
            quat.x = decodeFloat(bytes, c_nExtraSizeOffset+48);
            quat.y = decodeFloat(bytes, c_nExtraSizeOffset+52);
            quat.z = decodeFloat(bytes, c_nExtraSizeOffset+56);
            quat.w = decodeFloat(bytes, c_nExtraSizeOffset+60);
            
            float gforcex = decodeFloat(bytes, c_nExtraSizeOffset+64);
            float gforcey = decodeFloat(bytes, c_nExtraSizeOffset+68);
            float gforcez = decodeFloat(bytes, c_nExtraSizeOffset+72);            
           
            double pitch = Math.toDegrees(quat.toPitchFromYUp());
            double yaw = Math.toDegrees(quat.toYawFromYUp());
            double roll = Math.toDegrees(quat.toRollFromYUp());
            
            System.out.println("Telemetry:");
            
            System.out.print("  State: " + state);
            if (inPlay)
            {
              System.out.print(" (Play Mode)");
            }
            if (onboard)
            {
              System.out.print(" (Onboard)");
            }
            if (paused)
            {
              System.out.print(" (Paused)");
            }
            System.out.println("");
            
            System.out.println("  Frame Number: " + frameNo);
            
            System.out.print("  View Mode: " + viewMode);
            if (viewMode == 1)
            {
              System.out.print(" (Ride View)");
            }
            System.out.println("");
            
            System.out.println("  Coaster Index: " + coasterIndex);
            System.out.println("  Coaster Style Id: " + coasterStyleId);
            System.out.println("  Current Train Index: " + currentTrain);
            System.out.println("  Current Car Index: " + currentCar);
            System.out.println("  Current Seat Index: " + currentSeat);
            System.out.println("  Speed: " + speed + "m/s");
            System.out.println("  Position: " + posx + " " + posy + " " + posz);
            System.out.println("  Pitch: " + pitch + "deg");
            System.out.println("  Yaw: " + yaw + "deg");
            System.out.println("  Roll: " + roll + "deg");
            System.out.println("  G-forces: " + gforcex + " " + gforcey + " " + gforcez);
          }
          break;
        case N_MSG_INT_VALUE:
          if (size == 4)
          {
            System.out.println("Int value: " + decodeInt32(bytes, c_nExtraSizeOffset));
          }
          break;
        case N_MSG_INT_VALUE_PAIR:
          if (size == 8)
          {
            System.out.println("Int value pair: " + decodeInt32(bytes, c_nExtraSizeOffset) + ", " + decodeInt32(bytes, c_nExtraSizeOffset+4));
          }
          break;
        case N_MSG_STATION_STATE:
          if (size == 4)
          {
            int nState = decodeInt32(bytes, c_nExtraSizeOffset);
            
            boolean bEStop = (nState & (1<<0)) != 0;
            boolean bManualDispatch = (nState & (1<<1)) != 0;
            boolean bCanDispatch = (nState & (1<<2)) != 0;
            boolean bCanCloseGates = (nState & (1<<3)) != 0;
            boolean bCanOpenGates = (nState & (1<<4)) != 0;
            boolean bCanCloseHarness = (nState & (1<<5)) != 0;
            boolean bCanOpenHarness = (nState & (1<<6)) != 0;
            boolean bCanRaisePlatform = (nState & (1<<7)) != 0;
            boolean bCanLowerPlatform = (nState & (1<<8)) != 0;
            boolean bCanLockFlyerCar = (nState & (1<<9)) != 0;
            boolean bCanUnlockFlyerCar = (nState & (1<<10)) != 0;
            
            System.out.print("Station state: ");
            System.out.println("    E-Stop: " + bEStop);
            System.out.println("    Manual Dispatch: " + bManualDispatch);
            System.out.println("    Can Dispatch: " + bCanDispatch);
            System.out.println("    Can Close Gates: " + bCanCloseGates);
            System.out.println("    Can Open Gates: " + bCanOpenGates);
            System.out.println("    Can Close Harness: " + bCanCloseHarness);
            System.out.println("    Can Open Harness: " + bCanOpenHarness);
            System.out.println("    Can Raise Platform: " + bCanRaisePlatform);
            System.out.println("    Can Lower Platform: " + bCanLowerPlatform);
            System.out.println("    Can Lock Flyer Car: " + bCanLockFlyerCar);
            System.out.println("    Can Unlock Flyer Car: " + bCanUnlockFlyerCar);            
          }
          break;
        default:
          System.out.println("Unknown message");
          break;
        }
      }
    }
  }
  
  /////
  
  /**
   * Helper class to decode rotation quaternion into pitch/yaw/roll
   */
  static class Quaternion
  {
    public double x;
    public double y;
    public double z;
    public double w;
    
    public double toPitchFromYUp()
    {
      double vx = 2*(x*y + w*y); 
      double vy = 2*(w*x - y*z);
      double vz = 1.0 - 2*(x*x + y*y);
      
      return Math.atan2(vy, Math.sqrt(vx*vx + vz*vz));
    }
    
    public double toYawFromYUp()
    {
      return Math.atan2(2*(x*y + w*y), 1.0 - 2*(x*x + y*y));
    }
    
    public double toRollFromYUp()
    {
      return Math.atan2(2*(x*y + w*z), 1.0 - 2*(x*x + z*z));
    }
  }
  
}

