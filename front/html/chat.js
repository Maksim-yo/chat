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

let peerId = null;
let peerName = null;

let isInitialized = false;

chatsMap = new Map();
roomsMap = new Map();
function onMessage(message) {

    switch(message.code) {
        
        case CODE_INFO:
            isInitialized = true;
            peerId = message.peerId;
            peerName = message.peerNickname;
   
            if(message.chats && message.chats.length > 0) {
                for (let index = 0; index < message.chats.length; index++) {
                    let chat = message.chats[index];
                    let newRoom = {
                        id: chat.id,
                        messages: chat.history,
                        peers: chat.peers,
                        // TODO: change naming of room
                        roomName: chat.peers[0].peerName === peerName ? chat.peers[1].peerName : chat.peers[0].peerName,
                        profile_image: "https://randomuser.me/api/portraits/men/1.jpg"
                    }
                    console.log(peerName);
                    pushNewConverstation(newRoom);
                }
            }
            break;

        case CODE_PEER_JOINED:
            break;

        case CODE_PEER_LEFT:
            break;           

        case CODE_PEER_MESSAGE:
            addMessageToRoom(message);
            break;

        case CODE_PEER_IS_TYPING:
            break;

        case CODE_PEER_MESSAGE_FILE:
            break;

        case CODE_FILE_REQUEST_CHUNK:
            break;

    }
}

socket.onclose = function(event) {
    console.log("Socket closed");
};

socket.onmessage = function(event) {
    onMessage(JSON.parse(event.data));
}
socket.onopen = function(){
    setTimeout(() => {}, 100)

};
function socketSendNextData(data) {
    socket.send(data);
}

function postChatMessage(message) {


}
function handleSumbitButton() {
    
    let outgoingMessage = document.getElementById("chatTextArea").value;
    let text = outgoingMessage.replace(/\s/g,'');   
    if(text !== "") {
        let chatMessage = {
            id: getCurrentRoomId(),
            peerId: peerId,
            peerNickname: peerName,
            code: CODE_PEER_MESSAGE,
            message: {
                peerId: peerId,
                peerNickname: peerName, 
                code: 3,
                message: outgoingMessage
            },
            peers: getCurrentRoomPeers(),
            history: []
        }
        document.getElementById("chatTextArea").value = "";
        socketSendNextData(JSON.stringify(chatMessage));
    }

    return false;

};


async function fetchChatMessages(count) {
    let url = "getChatMessages/${count}";
    return await new Promise(function (resolve, reject) {

      fetch(url)
        .then((response) => response.text())
        .then((data) => {
          resolve(getChatMessages(data));
        })

    });
  }

  function getChatMessages(json) {
    let chatMessages = JSON.parse(json);
    return chatMessages;

  }
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
        currentRoom: null,
        push(elm) {
          this.converstationHistory[elm.id] =  elm;  
        },
        
        getLastUserMessage(chat) {
          return chat.messages[0];
        },

        changeCurrentRoom(item) {
            
            currentRoom = this.converstationHistory[item.id];
            this.currentChat = currentRoom.messages;
            this.currentRoom = currentRoom;

        },
        
        addMessageToRoom(item){
            this.currentChat.push(item.message);
        },
        getCurrentRoomId(){
            return currentRoom.id;
        },
        getCurrentRoomPeers(){
            return currentRoom.peers;
        },
        currentChat: [],

        isPeerIdCurrentUser(item){
          if (!peerId)
            return false;
          return item.peerId === peerId;
        },

      });
  })
  const pushNewConverstation = (elm) => {
    Alpine.store('converstationHistory').push(elm);

  }
  // document.onload = openNav();
    
  function handleChoiceClick(item) {
    Alpine.store('converstationHistory').changeCurrentRoom(item);
  }
  function getCurrentRoomId(){
    return Alpine.store('converstationHistory').getCurrentRoomId();
  }
  function getCurrentRoomPeers(){
    return Alpine.store('converstationHistory').getCurrentRoomPeers();
  }

  function addMessageToRoom(item){
    Alpine.store('converstationHistory').addMessageToRoom(item);

  }
