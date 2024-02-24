// TODO: refactor
let urlWebsocket = "ws://127.0.0.1:8000/ws  "
let socket = new WebSocket(urlWebsocket);

let CODE_INFO = 0;
let CODE_PEER_JOINED = 1;
let CODE_PEER_LEFT = 2;
let CODE_PEER_MESSAGE = 3;
let CODE_PEER_MESSAGE_FILE = 4;
let CODE_PEER_IS_TYPING = 5;
let CODE_PEER_READ = 6;
let CODE_PEER_DELIVERED = 7;
let CODE_FILE_SHARE = 8;
let CODE_FILE_REQUEST_CHUNK = 9;
let CODE_FILE_CHUNK_DATA = 10;

let CODE_FIND_ROOMS = 15;

let peerId = null;
let peerName = null;

let isInitialized = false;

chatsMap = {};
function makeDateMessage(prevDate, currentDate){
  if (prevDate == null)
    return currentDate.day + " " + currentDate.monthName
  let resString = currentDate.day + " " + currentDate.monthName;
  if (prevDate.year != currentDate.year || (prevDate.month - currentDate.month) > 3)
    resString += ", " + currentDate.year;
  return resString
} 

function onMessage(message) {
  if (message.peerId === peerId)
    return;
  switch (message.code) {

    case CODE_INFO:{
      isInitialized = true;
      peerId = message.peerId;
      peerName = message.peerNickname;
      if (message.chats && message.chats.length > 0) {
        for (let index = 0; index < message.chats.length; index++) {
          let chat = message.chats[index];
          let newChat = {
            id:  chat.id,
            peers: chat.peers,
            last_message: chat.history[Object.keys(chat.history).pop()],
            messages: [],
            // TODO: change naming of room
            room_name: chat.peers[0].peerName === peerName ? chat.peers[1].peerName : chat.peers[0].peerName,
            profile_image: "https://randomuser.me/api/portraits/men/1.jpg"
          }
          pushNewConverstation(newChat);
          let prevDate = null;

          for (let i = 0; i < chat.history.length; i++) {

            addMessageToChat(chat.history[i], chat.id);
          }
          
        }

      }
  }
  break;
  case CODE_PEER_READ:
  markMessagesAsRead(message.chat_id, message.count);
    break;
        case CODE_PEER_JOINED:
  break;

        case CODE_PEER_LEFT:
  break;           

        case CODE_PEER_MESSAGE:
          
              addMessageToChat(message.message, message.id);
  break;

        case CODE_PEER_IS_TYPING:
  break;

        case CODE_PEER_MESSAGE_FILE:
  break;

        case CODE_FILE_REQUEST_CHUNK:
  break;

        case CODE_FIND_ROOMS:

        if (message.users)
    setFindConversationResult(message.users);
  else
    setFindConversationResult(null);
  break;
}
}

socket.onclose = function (event) {
  console.log("Socket closed");
};

socket.onmessage = function (event) {
  onMessage(JSON.parse(event.data));
}

function socketSendNextData(data) {
  socket.send(data);
}


function handleSumbitButton() {

  let outgoingMessage = document.getElementById("chatTextArea").value;
  let text = outgoingMessage.replace(/\s/g, '');
  if (text !== "") {
    let chatMessage = {
      id: getCurrentChatId(),
      peerId: peerId,
      peerNickname: peerName,
      code: CODE_PEER_MESSAGE,
      timestamp: Math.floor(new Date() / 1000),
      message: {
        peerId: peerId,
        peerNickname: peerName,
        code: 3,
        message: outgoingMessage,
        is_read: false,
        timestamp: Math.floor(new Date() / 1000)
      },
      peers: getCurrentChatPeers(),
      history: []
    }
    addMessageToChat(chatMessage.message, getCurrentChatId());
    document.getElementById("chatTextArea").value = "";
    socketSendNextData(JSON.stringify(chatMessage));
    Alpine.nextTick(() => scrollToBottom())

  }

  return false;

};

function openNav() {
  document.getElementById("mySidenav").style.width = "250px";
  document.getElementById("menu-button").disabled = true;
  document.getElementById("menu-button").style.visibility = "hidden";
}

function closeNav() {
  document.getElementById("mySidenav").style.width = "0";
  setTimeout(function () {
    document.getElementById("menu-button").disabled = false;
    document.getElementById("menu-button").style.visibility = "visible ";
  }, 300);

}

document.body.addEventListener('keydown', function (e) {
  if (e.key == "Escape") {
    let isOpenMenu = document.getElementById("mySidenav").style.width === "0";
    if (!isOpenMenu)
      closeNav();
  }
});

document.addEventListener('alpine:init', () => {
  
  Alpine.store('converstationHistory',
    {
      converstationHistory: {},
      convertstationFindResult: [],
      currentChat: null,
      currentChatMessages: [],
      isFindResult: false,
      get getConverstationHistory() {
        console.log(this.isFindResult)
        if (this.isFindResult)
          return this.convertstationFindResult === null ? {} : this.convertstationFindResult;
        return this.converstationHistory;

      },
      push(elm) {
        this.converstationHistory[elm.id] = elm;
      },

      getLastUserMessage(chat) {
        return chat.messages[0];
      },

      changeCurrentChat(item) {

        currentRoom = this.converstationHistory[item.id];
        this.currentChat = currentRoom;
        this.currentChatMessages = currentRoom.messages;
      },

      setFindConversationResult(peers, flag) {
        this.convertstationFindResult = peers;
        this.isFindResult = flag;
      },

      addMessageToChat(message, roomId) {
        let chat = null;
        if (roomId === -1)
          chat = this.currentChat;
        else
          chat = this.converstationHistory[roomId];
        let lastMessage;
        if (chat.messages.length > 0)
          lastMessage = chat.messages[chat.messages.length - 1];
        if (lastMessage === undefined){
          chat.messages[0] = { 
            id: 0,
            is_date:true,
            message: makeDateMessage(null, convertSecondsToDate(message.timestamp))
          };
          message.id = 1;
        }
        else {
          lastMessageDate = convertSecondsToDate(lastMessage.timestamp);
          currentMessageDate = convertSecondsToDate(message.timestamp);
          message_id = lastMessage.id;
          if (lastMessageDate.day != currentMessageDate.day || lastMessageDate.month != currentMessageDate.month || lastMessageDate.year != currentMessageDate.year)
          {
            message_id++;
            chat.messages[message_id] = { 
            id: message_id,
            is_date:true,
            message: makeDateMessage(lastMessageDate, currentMessageDate)
          }; 
        }
          message.id = message_id + 1;
        }
        chat.messages[message.id] = message;
      },
      getCurrentChatId() {
        if (this.currentChat !== null)
          return this.currentChat.id;
        return null;
      },
      getCurrentChatPeers() {
        return this.currentChat.peers;
      },

      getFirstUnreadMessageIndex(chat_id = -1) {
        
        let chat = this.converstationHistory[chat_id];
        if (chat_id == -1)
          chat = this.currentChat;
        if (chat === undefined)
          return -1;
        let count = 0;
        for (let msg of chat.messages){
          if (msg.is_read === false)
            return count;
          count += 1;

        }
        return -1;
        
      },
      getFirstUnreadMessage(chat_id = -1){

        let chat = this.converstationHistory[chat_id];
        if (chat_id == -1)
          chat = this.currentChat;

        let index = this.getFirstUnreadMessageIndex(chat_id);
        if (index == -1)
          return null;
        return chat.messages[index];
      },

      markMessagesAsRead(chat_id, count){
        let chat = this.converstationHistory[chat_id];
        let messageIndex = this.getFirstUnreadMessageIndex(chat_id);
        let c = 0
        while (count != c){
          console.log(messageIndex + c);
          this.converstationHistory[chat_id].messages[messageIndex + c].is_read = true;
          c++;
        }
      },
      getUnreadMessagesCount(chat_id){
        let chat = this.converstationHistory[chat_id];
        if (chat_id == -1)
          chat = this.currentChat;
        if (chat === undefined)
          return -1;
        let msgCount = this.getFirstUnreadMessageIndex(chat_id);
        let msg = this.getFirstUnreadMessage(chat_id);
        if (msgCount === -1 || msg.peerId == peerId)
          return -1;
        return chat.messages.length - msgCount;
      },

      isPeerIdCurrentUser(item) {
        if (!peerId)
          return false;
        return item.peerId === peerId;
      },

    }),
    Alpine.store('scrollChatButton', {
      visible: false
    }),
    Alpine.data("searchInput", () => ({
      search: '',
      inputDelay: 700,
      inputTimer: null,

      handleSearchInput() {
        console.log(this.search)
        if (this.search === "") {
          setFindConversationResult(null, false);
          return;
        }
        this.sendInputMessage();

      },

      sendInputMessage() {
        console.log('message is sendign')
        console.log(this.search)
        let msg = {
          peerId: peerId,
          peerNickname: peerName,
          code: CODE_FIND_ROOMS,
          query: this.search,
          users: []
        }
        socketSendNextData(JSON.stringify(msg));
      },


      setInputTimer() {
        clearTimeout(this.inputTimer);
        this.inputTimer = setTimeout(() => this.handleSearchInput(), this.inputDelay);
      },

    })),
    Alpine.data("basePage", () => ({

      isBasePage: true,

    }));
})


const pushNewConverstation = (elm) => {
  Alpine.store('converstationHistory').push(elm);

}

function handleScroller(){
  let elm = document.getElementById("chat-scroller");
  let firstUnreadMessageIndex = Alpine.store('converstationHistory').getFirstUnreadMessageIndex();
  if (firstUnreadMessageIndex !== -1 && (elm.scrollHeight - elm.clientHeight) > 0) {

    let chatMessage = document.getElementsByClassName('chat-message')[firstUnreadMessageIndex-1];
    let messagePosition = chatMessage.getBoundingClientRect();
    elm.scrollTop = ( (elm.scrollTop + messagePosition.top) );

  }
  else 
    scrollToBottom();
};
function scrollingHanlder(){
  let elm = document.getElementById("chat-scroller");
  let messages = document.getElementsByClassName('chat-message');
  let lastUnreadMessageIndex = Alpine.store('converstationHistory').getFirstUnreadMessageIndex();
  let firstMessage = messages[lastUnreadMessageIndex - 1];
  let currentMessage = firstMessage;
  let count = 0;
  let msgPeerId = null; 
  if (lastUnreadMessageIndex != -1)
    msgPeerId = Alpine.store('converstationHistory').getFirstUnreadMessage().peerId;

  while (lastUnreadMessageIndex !== -1 && msgPeerId != peerId && currentMessage.getBoundingClientRect().bottom + elm.scrollTop <= elm.scrollTop + elm.clientHeight)
  {
    if (lastUnreadMessageIndex >= messages.length)
      break;
    count++;
    let message = messages[lastUnreadMessageIndex].querySelector('.span').textContent;
    currentMessage = messages[lastUnreadMessageIndex];
    lastUnreadMessageIndex++;
    markMessagesAsRead(getCurrentChatId(), 1);
  }

  
  let msg = {
    peerId: peerId,
    peerNickname: peerName,
    code: CODE_PEER_READ,
    count: count,
    chat_id: getCurrentChatId()
  }
  if (count > 0)
    socketSendNextData(JSON.stringify(msg));

    handleScrollButton()

}


function handleScrollButton(){
  let elm = document.getElementById("chat-scroller");
  let lastUnreadMessageIndex = Alpine.store('converstationHistory').getFirstUnreadMessageIndex();
  let lastUnreadMessage = Alpine.store('converstationHistory').getFirstUnreadMessage();
  let maxScroll = (elm.scrollHeight - elm.clientHeight);
  if ((maxScroll - elm.clientHeight) > elm.scrollTop || lastUnreadMessageIndex != -1 && lastUnreadMessage.peerId != peerId) {
    Alpine.store('scrollChatButton').visible = true;
  }
  else {
    
    Alpine.store('scrollChatButton').visible = false;
  }
  
}
function changeCurrentChat(item) {
  Alpine.store('converstationHistory').changeCurrentChat(item);

  
  Alpine.nextTick(() => handleScroller());
  Alpine.nextTick(() => scrollingHanlder());
}

function getCurrentChatId() {
  return Alpine.store('converstationHistory').getCurrentChatId();
}

function getCurrentChatPeers() {
  return Alpine.store('converstationHistory').getCurrentChatPeers();
}

function addMessageToChat(item, roomId = -1) {
  console.log(roomId);
  Alpine.store('converstationHistory').addMessageToChat(item, roomId);
  if (roomId != getCurrentChatId())
    return;
  let elm = document.getElementById("chat-scroller");
  var maxScroll = elm.scrollHeight - elm.clientHeight;
  if (maxScroll - 5 <= elm.scrollTop && elm.scrollTop <= maxScroll + 5)
    Alpine.nextTick(() => {scrollToBottom()});
  
  Alpine.nextTick(() => scrollingHanlder());


}

function getChatUnreadMessagesCount(chat_id = -1){
  return Alpine.store('converstationHistory').getUnreadMessagesCount(chat_id);
}

function scrollToBottom() {
    let elm = document.getElementById("chat-scroller");
    var maxScroll = elm.scrollHeight - elm.clientHeight;
    elm.scrollTop = maxScroll;

}

function setFindConversationResult(peers, flag = true) {
  Alpine.store('converstationHistory').setFindConversationResult(peers, flag);
}

function convertSecondsToDate(seconds){
  let date = new Date(seconds * 1000);
  return {
    minutes: date.getMinutes(),
    hours: date.getHours(),
    day: date.getDate(),
    monthName: date.toLocaleString('default', { month: 'long' }),
    month: date.getMonth,
    year: date.getFullYear()
  };
}

function getMessageStringDate(timestamp){
  date = convertSecondsToDate(timestamp);
  return date["hours"] + ":" + date["minutes"];
}

function markMessagesAsRead(chat_id, count){
  Alpine.store('converstationHistory').markMessagesAsRead(chat_id, count);
}