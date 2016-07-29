#include <linux/module.h>
#include <net/sock.h>
#include <linux/netlink.h>
#include <linux/skbuff.h>

#define NETLINK_USER 31

struct sock *nl_sk = NULL;

static void hello_nl_recv_msg(struct sk_buff *skb)
{
    struct nlmsghdr *nlh;
    struct sk_buff *skb_out;
    int pid;
    int res;
    char *msg = "Hello from kernel";
    int msg_size = strlen(msg);

    printk(KERN_INFO "Entering: %s\n", __FUNCTION__);

    nlh = (struct nlmsghdr *)skb->data;
    pid = nlh->nlmsg_pid;                               /*pid of sending process */
    printk(KERN_INFO "Netlink recv msg [%s] from %d\n", (char *)nlmsg_data(nlh), pid);

    skb_out = nlmsg_new(msg_size, 0);
    if (!skb_out) {
        printk(KERN_ERR "Allocate skb failed\n");
        return;
    }

    nlh = nlmsg_put(skb_out, 0, 0, NLMSG_DONE, msg_size, 0);
    NETLINK_CB(skb_out).dst_group = 0;                  /* not in mcast group */
    strncpy(nlmsg_data(nlh), msg, msg_size);

    res = nlmsg_unicast(nl_sk, skb_out, pid);
    if (res < 0)
        printk(KERN_INFO "Sending msg failed\n");
}

static int __init net_link_init(void)
{
    struct netlink_kernel_cfg cfg = {
        .input = hello_nl_recv_msg,
    };

    printk("Entering: %s\n", __FUNCTION__);
    nl_sk = netlink_kernel_create(&init_net, NETLINK_USER, &cfg);
    if (!nl_sk){
        printk(KERN_ALERT "Creating netlink socket failed\n");
        return -10;
    }

    return 0;
}

static void __exit net_link_exit(void)
{
    printk(KERN_INFO "Exiting: %s\n", __FUNCTION__);
    netlink_kernel_release(nl_sk);
}

module_init(net_link_init);
module_exit(net_link_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("hptsfb@gmail.com");
MODULE_DESCRIPTION("Testing code for netlink");
MODULE_VERSION("1.0.1");
