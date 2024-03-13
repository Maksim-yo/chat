// TODO: refactor
let urlWebsocket = 'ws://127.0.0.1:8000/ws  '
let socket = new WebSocket(urlWebsocket)

let CODE_INFO = 0
let CODE_PEER_JOINED = 1
let CODE_PEER_LEFT = 2
let CODE_PEER_MESSAGE = 3
let CODE_PEER_MESSAGE_FILE = 4
let CODE_PEER_IS_TYPING = 5
let CODE_PEER_READ = 6
let CODE_PEER_DELIVERED = 7
let CODE_FILE_SHARE = 8
let CODE_FILE_REQUEST_CHUNK = 9
let CODE_FILE_CHUNK_DATA = 10
let CODE_PEER_ONLINE = 14
let CODE_PEER_OFFLINE = 15
let CODE_FIND_ROOMS = 17

let peerId = null
let peerName = null

function initPeer(message) {
    peerId = message.peerId
    peerName = message.peerNickname
}

function createDateMessage(prevDate, currentDate) {
    if (prevDate == null) return currentDate.day + ' ' + currentDate.monthName
    let resString = currentDate.day + ' ' + currentDate.monthName
    if (
        prevDate.year != currentDate.year ||
        prevDate.month - currentDate.month > 3
    )
        resString += ', ' + currentDate.year
    return resString
}

function createChatMessage(chatMessage) {
    chat_name =
        chatMessage.peers[0].peerName === peerName
            ? chatMessage.peers[1].peerName
            : chatMessage.peers[0].peerName
    return {
        id: chatMessage.id,
        peers: chatMessage.peers,
        last_message:
            chatMessage.history[Object.keys(chatMessage.history).pop()],
        messages: [],
        room_name: chat_name,
        profile_image: 'https://randomuser.me/api/portraits/men/1.jpg',
    }
}

function makeOutgoingChatMessage(message) {
    return {
        peerId: peerId,
        peerNickname: peerName,
        code: CODE_PEER_MESSAGE,
        timestamp: Math.floor(new Date() / 1000),
        message: {
            peerId: peerId,
            message: message,
            is_read: false,
            timestamp: Math.floor(new Date() / 1000),
            chat_id: getCurrentChatId(),
        },
    }
}

function makeOutgoingReadMessage(count) {
    return {
        peerId: peerId,
        peerNickname: peerName,
        code: CODE_PEER_READ,
        count: count,
        chat_id: getCurrentChatId(),
    }
}

function makeOutgoingFindMessage(query) {
    return {
        peerId: peerId,
        peerNickname: peerName,
        code: CODE_FIND_ROOMS,
        query: search,
        users: [],
    }
}

function getMaxScroll() {
    let elm = document.getElementById('chat-scroller')
    var maxScroll = elm.scrollHeight - elm.clientHeight
    return maxScroll
}

function setMaxScroll(val) {
    let elm = document.getElementById('chat-scroller')
    elm.scrollTop = val
}

function onMessage(message) {
    switch (message.code) {
        case CODE_INFO:
            {
                initPeer(message)
                if (message.chats && message.chats.length > 0) {
                    for (let index = 0; index < message.chats.length; index++) {
                        let chat = message.chats[index]
                        let newChat = createChatMessage(chat)
                        pushNewConverstation(newChat)
                        for (let i = 0; i < chat.history.length; i++) {
                            addMessageToChat(chat.history[i], chat.id)
                        }
                    }
                }
            }
            break
        case CODE_PEER_READ:
            if (message.peerId !== peerId)
                markMessagesAsRead(message.chat_id, message.count)
            break
        case CODE_PEER_MESSAGE:
            addMessageToChat(message.message, message.message.chat_id)
            if (message.peerId === peerId)
                Alpine.nextTick(() => scrollToBottom())
            break
        case CODE_PEER_IS_TYPING:
            break
        case CODE_PEER_MESSAGE_FILE:
            break
        case CODE_FILE_REQUEST_CHUNK:
            break
        case CODE_FIND_ROOMS:
            if (message.users !== undefined)
                setFindConversationResult(message.users)
            else setFindConversationResult(null)
            break
        case CODE_PEER_ONLINE:
            changePeerStatus(message.peerId, true)
            break
        case CODE_PEER_OFFLINE:
            changePeerStatus(message.peerId, false)
            break
    }
}

socket.onclose = function (event) {
    console.log('Socket closed')
}

socket.onopen = function (event) {
    msg = { code: CODE_INFO }
    socketSendNextData(JSON.stringify(msg))
}

socket.onmessage = function (event) {
    onMessage(JSON.parse(event.data))
}

function socketSendNextData(data) {
    socket.send(data)
}

function handleSumbitButton() {
    let outgoingMessage = document.getElementById('chatTextArea').value
    let text = outgoingMessage.replace(/\s/g, '')
    if (text !== '') {
        let chatMessage = makeOutgoingChatMessage(outgoingMessage)
        document.getElementById('chatTextArea').value = ''
        socketSendNextData(JSON.stringify(chatMessage))
    }

    return false
}

document.body.addEventListener('keydown', function (e) {
    if (e.key == 'Escape') {
        window.dispatchEvent(new CustomEvent('navbar_esc', { bubbles: true }))
    }
})

document.addEventListener('alpine:init', () => {
    Alpine.store('chat', {
        converstationHistory: {},
        convertstationFindResult: [],
        currentChat: null,
        isFindResult: false,
        get getConverstationHistory() {
            if (this.isFindResult)
                return this.convertstationFindResult === null
                    ? {}
                    : this.convertstationFindResult
            return this.converstationHistory
        },
        push(elm) {
            this.converstationHistory[elm.id] = elm
        },

        getLastUserMessage(chat) {
            return chat.messages[0]
        },

        changeCurrentChat(item) {
            currentRoom = this.converstationHistory[item.id]
            this.currentChat = currentRoom
        },

        setFindConversationResult(peers, flag) {
            this.convertstationFindResult = peers
            this.isFindResult = flag
        },

        addMessageToChat(message, roomId) {
            let chat = null
            if (roomId === -1) chat = this.currentChat
            else chat = this.converstationHistory[roomId]
            let lastMessage
            if (chat.messages.length > 0) {
                lastMessage = chat.messages[chat.messages.length - 1]
                lastMessageDate = convertSecondsToDate(lastMessage.timestamp)
                currentMessageDate = convertSecondsToDate(message.timestamp)
                if (
                    lastMessageDate.day != currentMessageDate.day ||
                    lastMessageDate.month != currentMessageDate.month ||
                    lastMessageDate.year != currentMessageDate.year
                ) {
                    chat.messages.push({
                        is_date: true,
                        message: createDateMessage(
                            lastMessageDate,
                            currentMessageDate
                        ),
                    })
                }
            } else {
                chat.messages.push({
                    is_date: true,
                    message: createDateMessage(
                        null,
                        convertSecondsToDate(message.timestamp)
                    ),
                })
            }

            chat.messages.push(message)
        },
        getCurrentChatId() {
            if (this.currentChat !== null) return this.currentChat.id
            return null
        },
        getCurrentChatPeers() {
            if (this.currentChat !== null) return this.currentChat.peers
            return null
        },

        getFirstUnreadMessageIndex(chat_id = -1) {
            let chat = this.converstationHistory[chat_id]
            if (chat_id == -1) chat = this.currentChat
            if (chat === undefined) return -1
            for (let [index, msg] of chat.messages.entries()) {
                if (msg.is_read === false) return index
            }
            return -1
        },
        getFirstUnreadMessage(chat_id = -1) {
            let chat = this.converstationHistory[chat_id]
            if (chat_id == -1) chat = this.currentChat

            let index = this.getFirstUnreadMessageIndex(chat_id)
            if (index == -1) return null
            return chat.messages[index]
        },

        markMessagesAsRead(chat_id, count) {
            let chat = this.converstationHistory[chat_id]
            let messageIndex = this.getFirstUnreadMessageIndex(chat_id)
            let c = 0
            while (count != c) {
                this.converstationHistory[chat_id].messages[
                    messageIndex + c
                ].is_read = true
                c++
            }
        },
        getUnreadMessagesCount(chat_id) {
            let chat = this.converstationHistory[chat_id]
            if (chat_id == -1) chat = this.currentChat
            if (chat === undefined) return -1
            let msgCount = this.getFirstUnreadMessageIndex(chat_id)
            let msg = this.getFirstUnreadMessage(chat_id)
            if (msgCount === -1 || msg.peerId == peerId) return -1
            return chat.messages.length - msgCount
        },
    }),
        Alpine.store('scrollChatButton', {
            visible: false,
        }),
        Alpine.data('searchInput', () => ({
            search: '',
            inputDelay: 700,
            inputTimer: null,

            handleSearchInput() {
                if (this.search === '') {
                    setFindConversationResult(null, false)
                    return
                }
                this.sendInputMessage()
            },

            sendInputMessage() {
                socketSendNextData(
                    JSON.stringify(makeOutgoingFindMessage(this.search))
                )
            },

            setInputTimer() {
                clearTimeout(this.inputTimer)
                this.inputTimer = setTimeout(
                    () => this.handleSearchInput(),
                    this.inputDelay
                )
            },
        })),
        Alpine.data('navBar', () => ({
            visible: true,
            disable: false,
            width: 0,

            open() {
                this.visible = false
                this.disable = true
                this.width = 250
            },
            close() {
                this.width = 0
                setTimeout(() => {
                    this.visible = true
                    this.disable = false
                }, 100)
            },
        }))
})

const pushNewConverstation = (elm) => {
    Alpine.store('chat').push(elm)
}

function handleScroller() {
    let elm = document.getElementById('chat-scroller')
    let firstUnreadMessageIndex =
        Alpine.store('chat').getFirstUnreadMessageIndex()
    let firstUnreadMessage = Alpine.store('chat').getFirstUnreadMessage()
    if (
        firstUnreadMessageIndex !== -1 &&
        elm.scrollHeight - elm.clientHeight > 0 &&
        firstUnreadMessage.peerId != peerId
    ) {
        let chatMessage =
            document.getElementsByClassName('chat-message')[
                firstUnreadMessageIndex - 1
            ]
        let messagePosition = chatMessage.getBoundingClientRect()
        elm.scrollTop = elm.scrollTop + messagePosition.top
    } else scrollToBottom()
}

function scrollingHanlder() {
    let elm = document.getElementById('chat-scroller')
    let messages = document.getElementsByClassName('chat-message')
    let lastUnreadMessageIndex =
        Alpine.store('chat').getFirstUnreadMessageIndex()
    if (lastUnreadMessageIndex === -1) return
    let firstMessage = messages[lastUnreadMessageIndex - 1]
    let currentMessage = firstMessage
    let count = 0
    let msgPeerId = null
    msgPeerId = Alpine.store('chat').getFirstUnreadMessage().peerId

    while (
        msgPeerId != peerId &&
        currentMessage.getBoundingClientRect().bottom + elm.scrollTop <=
            elm.scrollTop + elm.clientHeight
    ) {
        if (lastUnreadMessageIndex >= messages.length) break
        count++
        currentMessage = messages[lastUnreadMessageIndex]
        lastUnreadMessageIndex++
        markMessagesAsRead(getCurrentChatId(), 1)
    }

    if (count > 0)
        socketSendNextData(JSON.stringify(makeOutgoingReadMessage(count)))

    handleScrollButton()
}

function handleScrollButton() {
    let elm = document.getElementById('chat-scroller')
    let lastUnreadMessageIndex =
        Alpine.store('chat').getFirstUnreadMessageIndex()
    let lastUnreadMessage = Alpine.store('chat').getFirstUnreadMessage()
    let maxScroll = elm.scrollHeight - elm.clientHeight
    if (
        maxScroll - elm.clientHeight > elm.scrollTop ||
        (lastUnreadMessageIndex != -1 && lastUnreadMessage.peerId != peerId)
    ) {
        Alpine.store('scrollChatButton').visible = true
    } else {
        Alpine.store('scrollChatButton').visible = false
    }
}
function changeCurrentChat(item) {
    Alpine.store('chat').changeCurrentChat(item)

    Alpine.nextTick(() => handleScroller())
    Alpine.nextTick(() => scrollingHanlder())
}

function getCurrentChatId() {
    return Alpine.store('chat').getCurrentChatId()
}

function getCurrentChatPeers() {
    return Alpine.store('chat').getCurrentChatPeers()
}

function addMessageToChat(item, roomId = -1) {
    Alpine.store('chat').addMessageToChat(item, roomId)
    if (roomId != getCurrentChatId()) return
    var maxScroll = getMaxScroll()
    let elm = document.getElementById('chat-scroller')
    if (maxScroll - 5 <= elm.scrollTop && elm.scrollTop <= maxScroll + 5)
        Alpine.nextTick(() => {
            scrollToBottom()
        })

    Alpine.nextTick(() => scrollingHanlder())
}

function getChatUnreadMessagesCount(chat_id = -1) {
    return Alpine.store('chat').getUnreadMessagesCount(chat_id)
}

function scrollToBottom() {
    var maxScroll = getMaxScroll()
    setMaxScroll(maxScroll)
}

function setFindConversationResult(peers, flag = true) {
    Alpine.store('chat').setFindConversationResult(peers, flag)
}

function convertSecondsToDate(seconds) {
    let date = new Date(seconds * 1000)
    return {
        minutes: date.getMinutes(),
        hours: date.getHours(),
        day: date.getDate(),
        monthName: date.toLocaleString('default', { month: 'long' }),
        month: date.getMonth,
        year: date.getFullYear(),
    }
}

function getMessageStringDate(timestamp) {
    date = convertSecondsToDate(timestamp)
    return date['hours'] + ':' + date['minutes']
}

function markMessagesAsRead(chat_id, count) {
    Alpine.store('chat').markMessagesAsRead(chat_id, count)
}

function changePeerStatus(peer_id, status) {
    for (let [index, chat] of Object.entries(
        Alpine.store('chat').converstationHistory
    )) {
        for (let peer of chat.peers) {
            if (peer.peerId == peer_id) peer.isOnline = status
        }
    }
}

function isPeerIdCurrentUser(item) {
    if (!peerId) return false
    return item.peerId === peerId
}

function test(peers) {
    for (let peer of peers) {
        if (peer.peerId != peerId) return peer.isOnline
    }
}
