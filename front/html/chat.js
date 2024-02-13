// TODO: refactor
let urlWebsocket = "ws://127.0.0.1:8000/ws  "
let socket = new WebSocket(urlWebsocket);

let CODE_INFO = 0;
let CODE_PEER_JOINED = 1;
let CODE_PEER_LEFT = 2;
let CODE_PEER_MESSAGE = 3;
let CODE_PEER_MESSAGE_FILE = 4;
let CODE_PEER_IS_TYPING = 5;

let CODE_FILE_SHARE = 6;
let CODE_FILE_REQUEST_CHUNK = 7;
let CODE_FILE_CHUNK_DATA = 8;

let CODE_FIND_ROOMS = 15;

let peerId = null;
let peerName = null;

let isInitialized = false;

chatsMap = {};

function onMessage(message) {

  switch (message.code) {

    case CODE_INFO:{
      isInitialized = true;
      peerId = message.peerId;
      peerName = message.peerNickname;
      if (message.chats && message.chats.length > 0) {
        for (let index = 0; index < message.chats.length; index++) {
          let chat = message.chats[index];
          let newChat = {
            id: chat.id,
            peers: chat.peers,
            last_message: chat.history[Object.keys(chat.history).pop()],
            messages: [],
            // TODO: change naming of room
            room_name: chat.peers[0].peerName === peerName ? chat.peers[1].peerName : chat.peers[0].peerName,
            profile_image: "https://randomuser.me/api/portraits/men/1.jpg"
          }
          // console.log(newChat)
          // console.log(chat)
          pushNewConverstation(newChat);
          for (let i = 0; i < chat.history.length; i++) {
            chat.history[i].is_read = true;
            addMessageToChat(chat.history[i], chat.id);
          }
          for (let i = 0; i < 16; i++) {

            addMessageToChat({
              code: 3,
              message: 'read',
              peerId: 2,
              is_read: true,
            }, chat.id);
          }
          for (let i = 0; i <11 ; i++) {

            addMessageToChat({
              code: 3,
              message: 'unread',
              peerId: 2,
              is_read: false,
            }, chat.id);
          }
        }

      }
  }
  break;

        case CODE_PEER_JOINED:
  break;

        case CODE_PEER_LEFT:
  break;           

        case CODE_PEER_MESSAGE:

  if (message.peerId !== peerId)
    addMessageToChat(message);
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
socket.onopen = function () {
  setTimeout(() => { }, 100)

};
function socketSendNextData(data) {
  socket.send(data);
}

function postChatMessage(message) {


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
      message: {
        peerId: peerId,
        peerNickname: peerName,
        code: 3,
        message: outgoingMessage
      },
      peers: getCurrentChatPeers(),
      history: []
    }
    addMessageToChat(chatMessage.message);
    document.getElementById("chatTextArea").value = "";
    socketSendNextData(JSON.stringify(chatMessage));
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
        if (lastMessage === undefined)
          message.id = 0;
        else
          message.id = lastMessage.id + 1;
        chat.messages[message.id] = message;
      },
      getCurrentChatId() {
        return this.currentChat.id;
      },
      getCurrentChatPeers() {
        return this.currentChat.peers;
      },

      getLastUnreadMessageIndex() {

        let count = 0;
        if (!this.currentChat || !this.currentChat.messages)
          return -1;
        for (let msg of this.currentChat.messages){
          count += 1;
          if (msg.is_read == false)
            return count;
        }
        return -1;
        
      },

      isPeerIdCurrentUser(item) {
        if (!peerId)
          return false;
        return item.peerId === peerId;
      },

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

function changeCurrentChat(item) {
  Alpine.store('converstationHistory').changeCurrentChat(item);

  function handleScroller(){
    let elm = document.getElementById("chat-scroller");
    let lastUnreadMessageIndex = Alpine.store('converstationHistory').getLastUnreadMessageIndex();
    if (lastUnreadMessageIndex != -1 || (elm.scrollHeight - elm.clientHeight) > 0) {
      let chatMessage = document.getElementsByClassName('chat_message')[0];
      let chatMessageStyle = getComputedStyle(chatMessage);
      let chatMessageMargin = parseInt(chatMessageStyle.marginBottom)
      elm.scrollTop = (lastUnreadMessageIndex - 1) * (chatMessage.offsetHeight + chatMessageMargin) - elm.clientHeight;
  
    }
    else 
      scrollToBottom();
  }
  Alpine.nextTick(() => handleScroller());
}
function getCurrentChatId() {
  return Alpine.store('converstationHistory').getCurrentChatId();
}
function getCurrentChatPeers() {
  return Alpine.store('converstationHistory').getCurrentChatPeers();
}

function addMessageToChat(item, roomId = -1) {
  Alpine.store('converstationHistory').addMessageToChat(item, roomId);
  if (roomId != -1)
    return;
  Alpine.nextTick(() => scrollToBottom());

  

}

function getScrollerMessagesHeight(){


} 
function scrollToBottom() {
  let elm = document.getElementById("chat-scroller");

    var maxScroll = elm.scrollHeight - elm.clientHeight;
    elm.scrollTop = maxScroll;

  
}


function setFindConversationResult(peers, flag = true) {
  Alpine.store('converstationHistory').setFindConversationResult(peers, flag);
}


function temp(){
  let elm = document.getElementById("chat-scroller");
console.log(elm.scrollTop);
}